/**
 * CONFIG - Diskulator select disk image
 */

/**
 * Select an image
 */
bool diskulator_select(void)
{
    unsigned char num_entries;
    unsigned char e;
    bool ret = false;

    POKE(0x60F, 2);
    POKE(0x610, 2);

subdir:
    selector_done = false;
    screen_clear();
    bar_clear();

    screen_puts(0, 0, "    DISK IMAGES    ");

    screen_puts(0, 21, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19"
                       "PICK \xD9\xA5\xB3\xA3\x19"
                       "ABORT");
    screen_puts(24, 21, "                  ");

    screen_puts(0, 1, prefix);

    current_entry[0] = '\0';

    diskulator_open_directory(selected_host, prefix);

    num_entries = 0;
    while ((current_entry[0] != 0x7F) && (num_entries < FILES_MAXCOUNT))
    {
        diskulator_read_directory(selected_host, current_entry, FILE_MAXLEN);
        if (current_entry[0] == '.')
            continue;
        else
        {
            if (current_entry[0] == 0x7F)
                break;
            else
            {
                strncpy(files[num_entries], current_entry, FILE_MAXLEN);

                if (current_entry[strlen(current_entry) - 1] == '/')
                    screen_puts(0, num_entries + 2, "\x04");

                screen_puts(2, num_entries + 2, current_entry);
                num_entries++;
            }
        }
    }

    diskulator_close_directory(selected_host);

    e = 0;
    bar_clear();
    bar_show(e + 3);

    selector_done = false;

    while (1)
    {
        if (kbhit())
	  {
	    rtclr();
            k = cgetc();
	  }
	else if (OS.rtclok[2]>100)
	  {
	    bar_set_color(0x44);
	  }
	
        switch (k)
        {
        case 0x1C: // Up
        case '-':
            if (e > 0)
                e--;
            break;
        case 0x1D: // Down
        case '=':
            if (e < num_entries - 1)
                e++;
            break;
        case 95: // SHIFT + UP ARROW
            color_luminanceIncrease();
            break;
        case 124: // SHIFT + DOWN ARROW
            color_luminanceDecrease();
            break;
        case 92: // CTRL + LEFT ARROW
            color_hueDecrease();
            break;
        case 94: // CTRL + RIGHT ARROW
            color_hueIncrease();
            break;
        case 0x1B: // ESC
            selector_done = true;
            memset(path, 0, sizeof(path));
            bar_set_color(0x97);
            ret = false;
            goto image_done;
            break;
        case 0x9B: // Enter
            bar_set_color(0x97);

            if (files[e][strlen(files[e]) - 1] == '/') // subdir
            {
                selector_done = false;
                strcat(prefix, files[e]);
                k = 0;
                goto subdir;
            }

            memset(path, 0, sizeof(path));
            strcat(path, prefix);
            strcat(path, files[e]);
            ret = true;
            goto image_done;
            break;
        }
        if (k > 0)
        {
            bar_clear();
            bar_show(e + 3);
            k = 0;
        }
    }
image_done:
    return ret;
}
