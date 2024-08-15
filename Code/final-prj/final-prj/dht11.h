/*
 * dht11.h
 *
 * Created: 5/22/2024 2:00:26 PM
 *  Author: Duong
 */ 


#ifndef DHT11_H_
#define DHT11_H_

unsigned char dth11_read(unsigned char *temperature, unsigned char *humidity) {
	unsigned char data[5] = {0};
	unsigned char checksum = 0;
	
	// Start condition
	DDRD |= (1 << dht11_pin); // Set pin as output
	PORTD &= ~(1 << dht11_pin); // Pull the pin low for at least 18ms
	_delay_ms(20);
	PORTD |= (1 << dht11_pin); // Pull the pin high and wait for the sensor response
	_delay_us(30);
	DDRD &= ~(1 << dht11_pin); // Set pin as input
	
	// dht11 response
	DDRD &= ~(1 << dht11_pin);
	
	while ((PIND & (1 << dht11_pin))); // Wait for the sensor to pull the pin low
	while (!(PIND & (1 << dht11_pin))); // Wait for the sensor to pull the pin high
	while ((PIND & (1 << dht11_pin))); // Wait for the sensor to pull the pin low
	
	// Read 5 bytes of data
	for (int i = 0; i < 5; i++) {
		for (int j = 7; j >= 0; j--) {
			while (!(PIND & (1 << dht11_pin))); // Wait for the sensor to pull the pin high
			_delay_us(40); // Delay for 40us
			if (PIND & (1 << dht11_pin)) // If the pin is still high after the delay
			{
				data[i] |= (1 << j); // Set the corresponding bit in the data byte
				while (PIND & (1 << dht11_pin)); // Wait for the sensor to pull the pin low again
			}
		}
	}
	
	// Verify checksum
	checksum = data[0] + data[1] + data[2] + data[3];
	if (checksum != data[4])
	return 0; // Checksum error
	
	// Extract temperature and humidity
	*humidity = data[0];
	*temperature = data[2];
	
	return 1; // Data read successfully
}



#endif /* DHT11_H_ */