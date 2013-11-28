/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Keyboard driver
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#include "system.h"

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */

#define KEY_BCKSPC		14
#define KEY_TAB			15
#define KEY_RETURN		28
#define KEY_CTRL		29
#define KEY_LSHIFT		42
#define KEY_RSHIFT		54
#define KEY_ALT			56
#define KEY_CAPSLOCK	58
#define KEY_F1			59
#define KEY_F2			60
#define KEY_F3			61
#define KEY_F4			62
#define KEY_F5			63
#define KEY_F6			64
#define KEY_F7			65
#define KEY_F8			66
#define KEY_F9			67
#define KEY_F10			68
#define KEY_F11			87
#define KEY_F12			88
#define KEY_NUMLOCK		69
#define KEY_SCRLLLOCK	70
#define KEY_HOME		71
#define KEY_PAGEUP		73
#define KEY_PAGEDOWN	81
#define KEY_UP			72
#define KEY_LEFT		75
#define KEY_RIGHT		77
#define KEY_DOWN		80
#define KEY_END			79
#define KEY_INSERT		82
#define KEY_DELETE		83

#define KEY_F_NONE	(0)
#define KEY_F_SHIFT (1<<0)
#define KEY_F_CTRL  (1<<1)
#define KEY_F_ALT	(1<<2)

unsigned short kbmod = KEY_F_NONE;

unsigned char kbdus[128] =
{
      0,  27, '1', '2', '3', '4', '5', '6', 
	'7', '8', '9', '0', '-', '=', '\b',	/* Backspace */ '\t',			/* Tab */
    'q', 'w', 'e', 'r',	/* 19 */ 't', 'y', 'u', 'i', 
    'o', 'p', '[', ']', '\n',		/* Enter key */ 0, /* 29   - Control */ 'a', 's', 
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
   '\'', '`',   0, /* Left shift */ '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', /* 49 */ 'm', ',', '.', '/',   0, /* Right shift */ '*',
      0,	/* Alt */
    ' ',	/* Space bar */
      0,	/* Caps lock */
      0,	/* 59 - F1 key ... > */
      0,   0,   0,   0,   0,   0,   0,   0,
      0,	/* < ... F10 */
      0,	/* 69 - Num lock*/
      0,	/* Scroll Lock */
      0,	/* Home key */
      0,	/* Up Arrow */
      0,	/* Page Up */
    '-',
      0,	/* Left Arrow */
      0,
      0,	/* Right Arrow */
    '+',
      0,	/* 79 - End key*/
      0,	/* Down Arrow */
      0,	/* Page Down */
      0,	/* Insert Key */
      0,	/* Delete Key */
      0,   0,   0,
      0,	/* F11 Key */
      0,	/* F12 Key */
      0,	/* All other keys are undefined */
};

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);

#define SRF(f) ((scancode & 0x80) ? kbmod & ~(f) : kbmod | (f))
	switch(scancode & 0x7f)
	{
		case KEY_LSHIFT:
		case KEY_RSHIFT:
			kbmod = SRF(KEY_F_SHIFT);
			break;
		case KEY_CTRL:
			kbmod = SRF(KEY_F_CTRL);
			break;
		case KEY_ALT:
			kbmod = SRF(KEY_F_ALT);
			break;
	}
#undef SRF

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else if(kbdus[scancode] >= 'a' && kbdus[scancode] <= 'z')
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        putch((kbmod & KEY_F_SHIFT) ? (kbdus[scancode] + 'A' - 'a') : kbdus[scancode]);
    }
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}
