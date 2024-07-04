#define RIGHT 0x1C
#define LEFT 0x18

#define CURSOR_ON 2
#define CURSOR_OFF 0

#define BLINK_ON 1
#define BLINK_OFF 0

#define BL_ON 0x08
#define BL_OFF 0x00

#ifdef PCF8574A
#define pcf_address_write 0x70 | (A2_A1_A0 << 1)
#define pcf_address_read 0x71 | (A2_A1_A0 << 1)
#else // PCF8574
#define pcf_address_write 0x40 | (A2_A1_A0 << 1)
#define pcf_address_read 0x41 | (A2_A1_A0 << 1)
#endif

#ifdef LCD20X4
#define line1_start 0x00
#define line1_end 0x13

#define line2_start 0x40
#define line2_end 0x53

#define line3_start 0x14
#define line3_end 0x27

#define line4_start 0x54
#define line4_end 0x67

#define line1_start_command 0x80
#define line2_start_command 0xC0
#define line3_start_command 0x94
#define line4_start_command 0xD4
#else LCD16X2
#define line1_start 0x00
#define line1_end 0x27

#define line2_start 0x40
#define line2_end 0x67

#define line1_start_command 0x80
#define line2_start_command 0xC0
#endif

int backlight_lcd_i2c = 0x08, lcd_i2c_line = 1;

void initialize_lcd_i2c()
{
    i2c_start();                  // Start communication
    i2c_write(pcf_address_write); // Send PCF address + WRITE bit
    i2c_write(0x2C);              // Configure LCD for 4 bits/BL ON/En 1
    i2c_write(0x28);              // En 0 to execute instruction

    delay_us(40);    // Verify the delay for bits to be sent
    i2c_write(0x2C); // Send first nibble of
    i2c_write(0x28); // the "Function Set" instruction.
    i2c_write(0x8C); // Send second nibble of the instruction.
    i2c_write(0x88); // 4 bits, 2 lines, font 5*8.

    delay_us(40);

    i2c_write(0x0C); // Send first nibble of
    i2c_write(0x08); // "Display ON/OFF control" instruction.
    i2c_write(0xCC); // Send second nibble of the instruction.
    i2c_write(0xC8); // Display on, cursor off, blink off.
    delay_us(1);

    i2c_write(0x0C); // Send first nibble of
    i2c_write(0x08); // "Entry Mode Set" instruction.
    i2c_write(0x6C); // Send second nibble of the instruction.
    i2c_write(0x68); // Increment cursor with each write
                     // (position+=1), LCD scroll disabled;
                     // if enabled, display scrolls with each character.
    delay_us(40);

    i2c_stop();
}
void send_lcd_i2c_command(char character)
{
    int ms_nibble_e, ls_nibble_e, ms_nibble, ls_nibble;

    ms_nibble = character & 0xF0;   // Most significant nibble of the character.
    ls_nibble = character & 0x0F;   // Least significant nibble of the character.
    swap(ls_nibble);                // Swap as the most significant bits are used (D7: D4).
    ms_nibble |= backlight_lcd_i2c; // Add BL to the most significant nibble
    ls_nibble |= backlight_lcd_i2c; // Add BL to the least significant nibble

    ms_nibble_e = ms_nibble | 0x04; // Most significant nibble En 1
    ls_nibble_e = ls_nibble | 0x04; // Least significant nibble En 1

    i2c_start();
    i2c_write(pcf_address_write);

    i2c_write(ms_nibble_e); // Send 1st nibble with "E" set
    i2c_write(ms_nibble);   // Send 1st nibble with "E" cleared

    i2c_write(ls_nibble_e); // Send 2nd nibble with "E" set
    i2c_write(ls_nibble);   // Send 2nd nibble with "E" cleared

    i2c_stop();
    delay_us(44);
}
void send_lcd_i2c_character(int character)
{
    int ms_nibble_e, ls_nibble_e, ms_nibble, ls_nibble;

    ms_nibble = character & 0xF0; // Most significant nibble of the character.
    ls_nibble = character & 0x0F; // Least significant nibble of the character.
    swap(ls_nibble);              // Swap as the most significant bits are used (D7: D4).

    ms_nibble |= backlight_lcd_i2c; // Add BL to the most significant nibble
    ls_nibble |= backlight_lcd_i2c; // Add BL to the least significant nibble
    ms_nibble++;                    // Set RS bit
    ls_nibble++;                    // Set RS bit

    ms_nibble_e = ms_nibble | 0x05; // Most significant nibble En and RS = 1
    ls_nibble_e = ls_nibble | 0x05; // Least significant nibble En and RS = 1

    i2c_start();
    i2c_write(pcf_address_write);

    i2c_write(ms_nibble_e); // Send 1st nibble with RS and En 1
    i2c_write(ms_nibble);   // Send 1st nibble with RS 1 and E 0

    i2c_write(ls_nibble_e); // Send 2nd nibble with RS and E 1
    i2c_write(ls_nibble);   // Send 2nd nibble with RS 0 and E 0

    i2c_stop();
    delay_us(44);
}
void clear_lcd_i2c()
{
    send_lcd_i2c_command(0x01);
    delay_ms(2);
}
int read_cursor_address()
{
    int aux, aux2, address;

    aux = 0xF2 | backlight_lcd_i2c; // Set PCF pins for D7:D4 of LCD E=0 | RW=1 | RS=0.
    aux2 = aux;                     // Copy aux value

    i2c_start();
    i2c_write(pcf_address_write);
    i2c_write(aux);        // Set PCF pins for D7:D4 of LCD E=0 | RW=1 | RS=0.
    i2c_write(aux | 0x04); // En=1 for LCD to provide most significant nibble.
    i2c_start();
    i2c_write(pcf_address_read);
    address = 0x70 & i2c_read(0); // Read most significant nibble with mask 0x70.
    i2c_start();
    i2c_write(pcf_address_write);
    i2c_write(aux);        // Clear Enable pin of LCD
    i2c_write(aux | 0x04); // Set Enable pin of LCD to provide least significant nibble.
    i2c_start();
    i2c_write(pcf_address_read);
    aux = 0xF0 & i2c_read(0); // Read least significant nibble with mask 0xF0.
    i2c_start();
    i2c_write(pcf_address_write);
    i2c_write(aux2); // Clear Enable pin first
    aux2 &= 0xF8;
    i2c_write(aux2); // Clear RW pin to exit read mode.
    i2c_stop();

    swap(aux);      // Position least significant nibble correctly.
    address |= aux; // OR operation between least and most significant nibble.
    delay_us(45);
    return (address); // Return current cursor address.
}
void write_lcd_i2c(char character)
{
    int cursor_address;

    if (character == '\n')
    {          // Position cursor at the start of the next line.
#ifdef LCD20X4 // Using LCD 20 x 4
        switch (lcd_i2c_line)
        {
        case 1:
            send_lcd_i2c_command(line2_start_command);
            lcd_i2c_line = 2;
            break;
        case 2:
            send_lcd_i2c_command(line3_start_command);
            lcd_i2c_line = 3;
            break;
        case 3:
            send_lcd_i2c_command(line4_start_command);
            lcd_i2c_line = 4;
            break;
        case 4:
            send_lcd_i2c_command(line1_start_command);
            lcd_i2c_line = 1;
            break;
        }
#else // Using LCD 16 x 2
        switch (lcd_i2c_line)
        {
        case 1:
            send_lcd_i2c_command(line2_start_command);
            lcd_i2c_line = 2;
            break;
        case 2:
            send_lcd_i2c_command(line1_start_command);
            lcd_i2c_line = 1;
            break;
        }
#endif
    }
    else
    {
        cursor_address = read_cursor_address();
        send_lcd_i2c_character(character);
#ifdef LCD20X4 // Using LCD 20 x 4
        switch (lcd_i2c_line)
        {
        case 1:
            if (cursor_address == line1_end)
            {
                send_lcd_i2c_command(line2_start_command);
                lcd_i2c_line = 2;
            }
            break;
        case 2:
            if (cursor_address == line2_end)
            {
                send_lcd_i2c_command(line3_start_command);
                lcd_i2c_line = 3;
            }
            break;
        case 3:
            if (cursor_address == line3_end)
            {
                send_lcd_i2c_command(line4_start_command);
                lcd_i2c_line = 4;
            }
            break;
        case 4:
            if (cursor_address == line4_end)
            {
                send_lcd_i2c_command(line1_start_command);
                lcd_i2c_line = 1;
            }
            break;
        }
#else // Using LCD 16 x 2
        switch (lcd_i2c_line)
        {
        case 1:
            if (cursor_address == line1_end)
            {
                send_lcd_i2c_command(line2_start_command);
                lcd_i2c_line = 2;
            }
            break;
        case 2:
            if (cursor_address == line2_end)
            {
                send_lcd_i2c_command(line1_start_command);
                lcd_i2c_line = 1;
            }
            break;
        }
#endif
    }
}
void send_lcd_i2c_string(char *string)
{
    while (*string)
        write_lcd_i2c(*string++);
}
void set_lcd_i2c_cursor_position(char x, char y)
{
    switch (x)
    {
    case 1:                                    // Line 1
        send_lcd_i2c_command(line1_start + y); // Position command.
        lcd_i2c_line = 1;                      // Assign current line for write control.
        break;
    case 2:                                    // Line 2
        send_lcd_i2c_command(line2_start + y); // Position command.
        lcd_i2c_line = 2;                      // Assign current line for write control.
        break;
#ifdef LCD20X4                                 // Using LCD 20 x 4
    case 3:                                    // Line 3
        send_lcd_i2c_command(line3_start + y); // Position command.
        lcd_i2c_line = 3;                      // Assign current line for write control.
        break;
    case 4:                                    // Line 4
        send_lcd_i2c_command(line4_start + y); // Position command.
        lcd_i2c_line = 4;                      // Assign current line for write control.
        break;
#endif
    }
}
void configure_lcd_i2c(int cursor, int blink, int backlight)
{
    int aux;
    backlight_lcd_i2c = 0x08 * backlight; // Set backlight status.

    aux = 0x08 | (cursor << 1) | blink; // Combine bits to form the control byte
    send_lcd_i2c_command(aux);          // Send control command to the LCD
}
