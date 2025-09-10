void itoBCD(int num, char* buffer) {

    // Variables used
	int numLeft = num;
	int power = 0;
	char BCDval[4];
	int tenToThe;
	int numTo;

    //Gets highest power of ten
	while (numLeft > 0) {
		numLeft = (int)(numLeft / 10);
		power++;
	}
	numLeft = num;

    /* This for loop is used to convert an integer into binary coded decimal
    *  Determines the digit in each place starting with the highest power of ten
    *  in the number. When a digit is found, the correct binary coded decimal value
    *  is selected and input into a string
    */
	for (int count = 0; count < power; count++) {
		numTo = numLeft;
		tenToThe = 1;
		for (int j = 0; j < power-count-1; j++) {
			tenToThe = tenToThe * 10;
		}
		numLeft = numLeft % tenToThe;
		numTo = (numTo - numLeft)/tenToThe;
		// Switch-Case determines what BCD value based on digit value
		if (numTo == 1) {
			BCDval[0] = '0';
			BCDval[1] = '0';
			BCDval[2] = '0';
			BCDval[3] = '1';
		}
		else if (numTo == 2) {
			BCDval[0] = '0';
			BCDval[1] = '0';
			BCDval[2] = '1';
			BCDval[3] = '0';
		}
		else if (numTo == 3) {
			BCDval[0] = '0';
			BCDval[1] = '0';
			BCDval[2] = '1';
			BCDval[3] = '1';
		}
		else if (numTo == 4) {
			BCDval[0] = '0';
			BCDval[1] = '1';
			BCDval[2] = '0';
			BCDval[3] = '0';
		}
		else if (numTo == 5) {
			BCDval[0] = '0';
			BCDval[1] = '1';
			BCDval[2] = '0';
			BCDval[3] = '1';
		}
		else if (numTo == 6) {
			BCDval[0] = '0';
			BCDval[1] = '1';
			BCDval[2] = '1';
			BCDval[3] = '0';
		}
		else if (numTo == 7) {
			BCDval[0] = '0';
			BCDval[1] = '1';
			BCDval[2] = '1';
			BCDval[3] = '1';
		}
		else if (numTo == 8) {
			BCDval[0] = '1';
			BCDval[1] = '0';
			BCDval[2] = '0';
			BCDval[3] = '0';
		}
		else if (numTo == 9) {
			BCDval[0] = '1';
			BCDval[1] = '0';
			BCDval[2] = '0';
			BCDval[3] = '1';
		}
		else if (numTo == 0){
			BCDval[0] = '0';
			BCDval[1] = '0';
			BCDval[2] = '0';
			BCDval[3] = '0';
		}

		// Adds BCD value of digit into string, appends onto end
		for (int j = 0; j < 4; j++) {
			buffer[j + 4*count] = BCDval[j];
	
		}
	}

	// Adds null terminator to end of string
	buffer[4 * power] = '\0';
}
