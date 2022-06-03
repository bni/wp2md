#include <stdio.h>

typedef enum { F, T } boolean;

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}

boolean checkCenteringStart(boolean centeringOn)
{
    if (centeringOn == T) {
        printf("<p style=\"text-align: center;\">");
    }

    return F;
}

void checkCenteringStop(FILE *f)
{
    // Peek if next is an "End of centered text"
    if (fpeek(f) == 0x83) {
        printf("</p>");
    }
}

int main(int argc, char* argv[])
{
	FILE *f;
	unsigned char c;

    boolean centeringOn = F;

    if (argc != 2) {
        printf("Usage: wp2md file\n");

        return 1;
    }

	f = fopen(argv[1], "rb");

	// Read all bytes
    while (fread(&c, sizeof(unsigned char), 1, f)) {
        checkCenteringStop(f);

        if (c == 0x80) {
            // No-op
        } else if (c == 0xc3) {
            // Center the following text
            centeringOn = T;

            // Skip over the following 4 that we dont care about
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
        } else if (c == 0x83) {
            // End of centered text
        } else if (c == 0x9d) {
            // Bold on
            printf("**");
        } else if (c == 0x9c) {
            // Bold off
            printf("**");
        } else if (c == 0xb2) {
            // Italics on
            printf("*");
        } else if (c == 0xb3) {
            // Italics off
            printf("*");
        } else if (c == 0x94) {
            // Underline on
            printf("<ins>");
        } else if (c == 0x95) {
            // Underline off
            printf("</ins>");
        } else if (c == 0xe1) {
            // Extended ascii
            fread(&c, sizeof(unsigned char), 1, f);
            printf("%c", c);
            // Skip the suffix
            fread(&c, sizeof(unsigned char), 1, f);
        } else if (c == 0x0a) {
            // Newline
            printf("\n");
        } else if (c >= 0x20 && c <= 0x7F) {
            // All printable
            centeringOn = checkCenteringStart(centeringOn);

            printf("%c", c);
        } else {
            // Not implemented function code
            printf("[%x]", c);
        }
	}

	fclose(f);
}
