void itoa(int num, char* buffer) {
	int numLeft;
	if (num < 0) {
		numLeft = -1 * num;
	}
	else {
		numLeft = num;
	}
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
	if (num < 0) {
		numLeft = -1 * num;
	}
	else {
		numLeft = num;
	}
	int numToChar;
	int tenToThePower;
	if (num < 0) {
		buffer[0] = '-';
		for (int i = 1; i < maxPower + 2; i++) {
			tenToThePower = 1;
			for (int j = 0; j < maxPower - i + 1; j++) {
				tenToThePower = tenToThePower * 10;
			}
			numToChar = (int)(numLeft / tenToThePower);
			buffer[i] = numToChar + '0';
			numLeft = numLeft - numToChar * tenToThePower;
		}
	}
	else {
		for (int i = 0; i < maxPower + 1; i++) {
			tenToThePower = 1;
			for (int j = 0; j < maxPower - i; j++) {
				tenToThePower = tenToThePower * 10;
			}
			numToChar = (int)(numLeft / tenToThePower);
			buffer[i] = numToChar + '0';
			numLeft = numLeft - numToChar * tenToThePower;
		}
	}

}
