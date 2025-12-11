#include <string.h>
#include <mpx/io.h>
#include <mpx/serial.h>
#include <mpx/interrupts.h>
#include <memory.h> 
#include <itoa.h>

extern void serial_isr(void *);

/************ PIC + UART DEFINES ************/

#define PIC_CMD  0x20
#define PIC_MASK 0x21
#define PIC_EOI  0x20

// UART register offsets (must match serial.c)
enum uart_registers {
    RBR = 0,  // Receive Buffer
    THR = 0,  // Transmitter Holding
    DLL = 0,  // Divisor Latch LSB
    IER = 1,  // Interrupt Enable
    DLM = 1,  // Divisor Latch MSB
    IIR = 2,  // Interrupt Identification
    FCR = 2,  // FIFO Control
    LCR = 3,  // Line Control
    MCR = 4,  // Modem Control
    LSR = 5,  // Line Status
    MSR = 6,  // Modem Status
    SCR = 7,  // Scratch
};

/************ DCBs PER DEVICE ************/

/* Standard PC base ports */
#define COM1_BASE 0x3F8
#define COM2_BASE 0x2F8
#define COM3_BASE 0x3E8
#define COM4_BASE 0x2E8

static dcb_t com1_dcb = { .device = COM1, .state = DEV_CLOSED, .open_status = closed, .allocation_status = unallocated, .event_flag = no_event, .base_port = COM1_BASE };
static dcb_t com2_dcb = { .device = COM2, .state = DEV_CLOSED, .open_status = closed, .allocation_status = unallocated, .event_flag = no_event, .base_port = COM2_BASE };
static dcb_t com3_dcb = { .device = COM3, .state = DEV_CLOSED, .open_status = closed, .allocation_status = unallocated, .event_flag = no_event, .base_port = COM3_BASE };
static dcb_t com4_dcb = { .device = COM4, .state = DEV_CLOSED, .open_status = closed, .allocation_status = unallocated, .event_flag = no_event, .base_port = COM4_BASE };

dcb_t *get_dcb_for_device(device dev)
{
    switch (dev) {
    case COM1: return &com1_dcb;
    case COM2: return &com2_dcb;
    case COM3: return &com3_dcb;
    case COM4: return &com4_dcb;
    default:   return NULL;
    }
}

static int irq_for_device(device dev)
{
    switch (dev) {
    case COM1: return 4;
    case COM2: return 3;
    case COM3: return 4;   // typical PC mapping
    case COM4: return 3;
    default:   return -1;
    }
}

/************ PIC HELPERS ************/

static void pic_mask_irq(int irq)
{
    if (irq < 0) return;
    cli();
    uint8_t mask = inb(PIC_MASK);
    mask |= (1 << irq);     // 1 = disable
    outb(PIC_MASK, mask);
    sti();
}

static void pic_unmask_irq(int irq)
{
    if (irq < 0) return;
    cli();
    
    /* debug prints removed */
    uint8_t mask = inb(PIC_MASK);
    mask &= ~(1 << irq);    // 0 = enable
    outb(PIC_MASK, mask);
    sti();
}

/************ RING BUFFER HELPERS ************/

static void ring_put(dcb_t *dcb, char c)
{
    if (dcb->ring_count >= RING_BUFFER_SIZE) {
        // Overflow: drop char (simple policy)
        return;
    }
    dcb->ring_buffer[dcb->ring_head] = c;
    dcb->ring_head = (dcb->ring_head + 1) % RING_BUFFER_SIZE;
    dcb->ring_count++;
}

static int ring_get(dcb_t *dcb, char *out)
{
    if (dcb->ring_count == 0) {
        return 0;
    }
    *out = dcb->ring_buffer[dcb->ring_tail];
    dcb->ring_tail = (dcb->ring_tail + 1) % RING_BUFFER_SIZE;
    dcb->ring_count--;
    return 1;
}


/************ OPEN / CLOSE ************/

int serial_open(device dev, int speed)
{
    dcb_t *dcb = get_dcb_for_device(dev);
    if (!dcb) {
        return -100;  // invalid device
    }

    int      irq  = irq_for_device(dev);
    uint16_t base = dcb->base_port;

    // Already open?
    if (dcb->open_status == open) {
        return -103;
    }

    // Baud divisor must divide 115200
    if (speed <= 0 || (115200 % speed) != 0) {
        return -102;
    }

    // Disable UART interrupts
    outb(base + IER, 0x00);

    // Set baud rate
    uint16_t divisor      = (uint16_t)(115200 / speed);
    uint8_t  divisor_low  = divisor & 0x00FF;
    uint8_t  divisor_high = (divisor >> 8) & 0x00FF;

    outb(base + LCR, 0x80);          // DLAB = 1
    outb(base + DLL, divisor_low);
    outb(base + DLM, divisor_high);
    outb(base + LCR, 0x03);          // 8 bits, no parity, 1 stop, DLAB=0

    // enable interrupts
    outb(base + MCR, 0x08);

    // Clear pending status
    (void)inb(base + LSR);
    (void)inb(base + RBR);
    (void)inb(base + IIR);
    (void)inb(base + MSR);

    // Initialize DCB
    memset(dcb->ring_buffer, 0, sizeof(dcb->ring_buffer));
    dcb->ring_head         = 0;
    dcb->ring_tail         = 0;
    dcb->ring_count        = 0;
    dcb->device            = dev;
    dcb->open_status       = open;
    dcb->allocation_status = allocated;
    dcb->state             = DEV_IDLE;
    dcb->event_flag        = no_event;
    dcb->q_head            = NULL;
    dcb->q_tail            = NULL;
    dcb->current           = NULL;

    // Install ISR and unmask IRQ
    if (irq >= 0) {
        idt_install(0x20 + irq, serial_isr);
        pic_unmask_irq(irq);
    }

    // Enable receive data available interrupts (bit 0)
    outb(base + IER, 0x01);

    return 0;
}

int serial_close(device dev)
{
    dcb_t *dcb = get_dcb_for_device(dev);
    if (!dcb || dcb->open_status != open) {
        return -201; // not open
    }

    uint16_t base = dcb->base_port;
    int      irq  = irq_for_device(dev);

    // Disable UART interrupts
    outb(base + IER, 0x00);

    // Mask PIC line
    pic_mask_irq(irq);

    dcb->open_status       = closed;
    dcb->allocation_status = unallocated;
    dcb->state             = DEV_CLOSED;
    dcb->event_flag        = no_event;
    dcb->ring_count        = 0;
    dcb->q_head            = NULL;
    dcb->q_tail            = NULL;
    dcb->current           = NULL;

    return 0;
}

/************ INPUT / OUTPUT INTERRUPT HELPERS ************/

static void serial_input_interrupt(dcb_t *dcb)
{
    uint16_t base = dcb->base_port;

    while (inb(base + LSR) & 0x01) {    // Data ready
        char c = inb(base + RBR);

        // Echo back the received character
        outb(base + THR, c);

        if (dcb->state == DEV_READING && dcb->current != NULL) {
            iocb_t *io = dcb->current;

            if (io->byte_count < io->size - 1) {
                io->buf[io->byte_count++] = c;
            }

            // Stop on CR/LF or full buffer
            if (c == '\r' || c == '\n' || io->byte_count >= io->size - 1) {
                // Remove CR/LF from stored buffer
                if (io->byte_count > 0 &&
                    (io->buf[io->byte_count - 1] == '\r' ||
                     io->buf[io->byte_count - 1] == '\n')) {
                    io->byte_count--;
                }
                io->buf[io->byte_count] = '\0';

                dcb->state      = DEV_IDLE;
                dcb->event_flag = unhandled_event;
            }
        } else {
            // No active read – stash into ring buffer
            ring_put(dcb, c);
        }
    }
}

static void serial_output_interrupt(dcb_t *dcb)
{
    uint16_t base = dcb->base_port;
    iocb_t  *io   = dcb->current;

    if (!io || dcb->state != DEV_WRITING) {
        // No active write; interrupts are off
        uint8_t ier = inb(base + IER);
        ier &= ~0x02;
        outb(base + IER, ier);
        return;
    }

    if (io->byte_count < io->size) {
        outb(base + THR, (uint8_t)io->buf[io->byte_count++]);
    }

    if (io->byte_count >= io->size) {
        uint8_t ier = inb(base + IER);
        ier &= ~0x02;
        outb(base + IER, ier);

        dcb->state      = DEV_IDLE;
        dcb->event_flag = unhandled_event;
    }
}

/************SERIAL ISR************/

void serial_interrupt(void)
{
    cli();  
    device devs[] = { COM1, COM2, COM3, COM4 };

    for (int i = 0; i < 4; ++i) {
        dcb_t *dcb = get_dcb_for_device(devs[i]);
        if (!dcb || dcb->open_status != open)
            continue;

        uint16_t base = dcb->base_port;
        uint8_t  iir  = inb(base + IIR);

        // Bit 0 == 1 => no interrupt pending
        if (iir & 0x01)
            continue;

        // Bits 3:1 = interrupt reason
        switch (iir & 0x0E) {
        case 0x04:  // RX data available
        case 0x0C:  // RX timeout
            serial_input_interrupt(dcb);
            break;

        case 0x02:  // TX
            serial_output_interrupt(dcb);
            break;

        default:
            // Clear
            (void)inb(base + LSR);
            (void)inb(base + RBR);
            (void)inb(base + MSR);
            break;
        }
    }

    // End Of Interrupt to PIC
    outb(PIC_CMD, PIC_EOI);

    sti();
}

/************ Read And Write ************/

int serial_read(device dev, char *buf, size_t len)
{
    dcb_t *dcb = get_dcb_for_device(dev);
    if (!dcb || dcb->open_status != open)
        return -301;    // invalid or not open

    if (!buf || len == 0)
        return -302;    // invalid buffer/len

    if (dcb->state != DEV_IDLE && dcb->state != DEV_READING)
        return -304;    // busy

    iocb_t *io = dcb->current;
    if (!io)
        return -304;    // scheduler error: no IOCB present

    io->buf        = buf;
    io->size       = len;
    io->byte_count = 0;

    dcb->state      = DEV_READING;
    dcb->event_flag = no_event;

    // Use any chars already in the ring buffer
    char c;
    while (io->byte_count < io->size - 1 && ring_get(dcb, &c)) {
        if (c == '\r' || c == '\n') {
            // End of line; don't store CR/LF
            io->buf[io->byte_count] = '\0';
            dcb->state              = DEV_IDLE;
            dcb->event_flag         = unhandled_event;
            return 0;
        }
        io->buf[io->byte_count++] = c;
    }

    // If buffer filled before newline, complete immediately
    if (io->byte_count >= io->size - 1) {
        io->buf[io->byte_count] = '\0';
        dcb->state              = DEV_IDLE;
        dcb->event_flag         = unhandled_event;
    }

    return 0;
}

int serial_write(device dev, char *buf, size_t len)
{
    dcb_t *dcb = get_dcb_for_device(dev);
    if (!dcb || dcb->open_status != open)
        return -401;                // invalid or not open

    if (!buf || len == 0)
        return -402;                // invalid buffer/len

    // Device must be idle or already writing (io_schedule sets this up)
    if (dcb->state != DEV_IDLE && dcb->state != DEV_WRITING)
        return -404;                // busy

    // I/O scheduler must have set up current IOCB
    iocb_t *io = dcb->current;
    if (!io)
        return -404;                // scheduler error: no IOCB present

    // Initialize IOCB for this write
    io->buf        = buf;
    io->size       = len;
    io->byte_count = 0;

    dcb->state      = DEV_WRITING;
    dcb->event_flag = no_event;

    uint16_t base = dcb->base_port;

    /*
     * Kick out the FIRST byte only (per R6 spec).
     * Do NOT spin in a while-loop forever; just send if THR is empty.
     * If it's not empty yet, the next TX interrupt will pick it up.
     */
    uint8_t lsr = inb(base + LSR);
    if ((lsr & 0x20) != 0 && io->size > 0) {
        outb(base + THR, (uint8_t)io->buf[0]);
        io->byte_count = 1;
    }

    /* Enable transmit holding register empty interrupts (bit 1 in IER) */
    uint8_t ier = inb(base + IER);
    ier |= 0x02;                    // enable TX interrupt
    outb(base + IER, ier);

    // Asynchronous: ISR will finish the transfer and set event_flag.
    return 0;
}
