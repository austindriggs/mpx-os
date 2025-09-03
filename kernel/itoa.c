void itoa(int num, char* buffer) {
	// get the absolute value of num
	int numLeft;
	if (num < 0) {
		numLeft = -1 * num;
	}
	else {
		numLeft = num;
	}

	// find the maximum power of 10 needed
	int maxPower = 0;
	while (numLeft > 1) {
		numLeft = numLeft / 10;
		if (numLeft < 1) {
			break;
		}
		else {
			maxPower++;
		}
	}

	// reset numLeft to the value of num
	if (num < 0) {
		numLeft = -1 * num;
	}
	else {
		numLeft = num;
	}

	// initialize variables
	int numToChar;
	int tenToThePower;

	// convert negative int to string
	if (num < 0) {
		buffer[0] = '-';
		for (int i = 1; i < maxPower + 2; i++) {
			// convert each int to char
			tenToThePower = 1;
			for (int j = 0; j < maxPower - i + 1; j++) {
				tenToThePower = tenToThePower * 10;
			}

			// write each char to buffer
			numToChar = (int)(numLeft / tenToThePower);
			buffer[i] = numToChar + '0';
			numLeft = numLeft - numToChar * tenToThePower;
		}
	}
	// convert positive int to string
	else {
		for (int i = 0; i < maxPower + 1; i++) {
			// convert each int to char
			tenToThePower = 1;
			for (int j = 0; j < maxPower - i; j++) {
				tenToThePower = tenToThePower * 10;
			}

			// write each char to buffer
			numToChar = (int)(numLeft / tenToThePower);
			buffer[i] = numToChar + '0';
			numLeft = numLeft - numToChar * tenToThePower;
		}
	}

	// add null terminator
	if (num < 0) {
		buffer[maxPower + 2] = '\0';
	}
	else {
		 buffer[maxPower + 1] = '\0';
	}
}
