#ifndef KBD_H
#define KBC_H

int kbc_read_status_buf(unsigned char *status);

int kbc_write_in_buf(unsigned char data);
int kbc_read_out_buf(unsigned char *data);

int kbc_send_command(unsigned char command);
int kbc_send_command_with_arg(unsigned char command, unsigned char arg);

int kdb_send_command(unsigned char command);
int kbd_send_command_with_arg(unsigned char command, unsigned char arg);

int mouse_send_command(unsigned char command);
int mouse_send_command_with_arg(unsigned char command, unsigned char arg);

int kbd_toggle_led(unsigned short led);
int kbd_clear_leds();

#endif
