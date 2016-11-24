#include "i8042.h"

.global _key
.global _kbd_int_handler_asm

.data
_key:
	.byte 0

.text
_kbd_int_handler_asm:
	inb $KBC_STATUS_PORT, %al	// ler o status presente na status port
	and $OUT_BUF_FULL, %al		// se o out buffer estiver vazio
	jz _kbd_int_handler_asm		// voltar ao inicio da funcao

	inb $KBC_OUT_PORT, %al		// ler o out buffer
	movb %al, _key				// alterar a key para o valor lido
	ret
