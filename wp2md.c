#include <stdio.h>

typedef enum { F, T } boolean;

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}

boolean checkAlignStart(boolean alignOn, char *type)
{
    if (alignOn == T) {
        printf("<p style=\"text-align: %s;\">", type);
    }

    return F;
}

void checkAlignStop(FILE *f)
{
    // Peek if next is an "End of centered text"
    if (fpeek(f) == 0x83 || fpeek(f) == 0x84) {
        printf("</p>");
    }
}

int main(int argc, char* argv[])
{
    FILE *f;
    unsigned char c;

    boolean centerOn = F, rightOn = F;

    if (argc != 2) {
        printf("Usage: wp2md file\n");

        return 1;
    }

    f = fopen(argv[1], "rb");

    // Read all bytes
    while (fread(&c, sizeof(unsigned char), 1, f)) {
        if (c == 0x80) {
            // No-op
        } else if (c == 0x09) {
            // Tab
            printf("&nbsp;&nbsp;&nbsp;"); // Not really a tab, Markdown doesnt support tabs
        } else if (c == 0xc3) {
            // Center the following text
            centerOn = T;

            // Skip over the following 4 that we dont care about
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
        } else if (c == 0xc4) {
            // Align or flush right
            rightOn = T;

            // Skip over the following 4 that we dont care about
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
            fread(&c, sizeof(unsigned char), 1, f);
        } else if (c == 0x83) {
            // End of centered text
        } else if (c == 0x84) {
            // End of aligned or flushed text
        } else if (c == 0x9d) {
            // Bold on
            printf("**");
        } else if (c == 0x9c) {
            // Bold off
            checkAlignStop(f);

            printf("**");
        } else if (c == 0xb2) {
            // Italics on
            printf("*");
        } else if (c == 0xb3) {
            // Italics off
            checkAlignStop(f);

            printf("*");
        } else if (c == 0x94) {
            // Underline on
            printf("<ins>");
        } else if (c == 0x95) {
            // Underline off
            checkAlignStop(f);

            printf("</ins>");
        } else if (c == 0xe1) {
            // Extended ascii
            fread(&c, sizeof(unsigned char), 1, f);
            printf("%c", c);
            // Skip the suffix
            fread(&c, sizeof(unsigned char), 1, f);
        } else if (c == 0x0a) {
            // Newline
            printf("\n\n"); // In Markdown two newlines or more indicate a linebreak
        } else if (c >= 0x20 && c <= 0x7F) {
            // All printable
            centerOn = checkAlignStart(centerOn, "center");
            rightOn = checkAlignStart(rightOn, "right");

            printf("%c", c);

            checkAlignStop(f);
        } else {
            // Not implemented function code
            printf("[%x]", c);
        }
    }

    fclose(f);
}
