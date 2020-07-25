 rehosts:
  // reset cursor
  i = 0;
  
 rehosts_jump:
  
  bar_clear();
  bar_show(i + 2);
  
  // Keyboard input in HOSTS area
  while (host_done == false)
    {
      // Quick boot
      
      if (kbhit())
        {
	  k=diskulator_host_process_key();
	  switch (k)
            {
            case 0x1C: // ATASCII UP
            case '-':
	      if (i > 0)
		i--;
	      break;
            case 0x1D: // ATASCII DOWN
            case '=':
	      if (i < 7)
		i++;
	      break;
            case '_': // SHIFT + UP ARROW
	      color_luminanceIncrease();
	      break;
            case '|': // SHIFT + DOWN ARROW
	      color_luminanceDecrease();
	      break;
            case '\\': // SHIFT + LEFT ARROW
	      color_hueDecrease();
	      break;
            case '^': // SHIFT + RIGHT ARROW
	      color_hueIncrease();
	      break;
            case '1': // Drives 1-8
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
	      i = k - '1';
	      goto jump_to_devs;
	      break;
            case 'C': // Config
            case 'c':
	      host_done = true;
	      slot_done = true;
	      ret = false;
	      info_run();
	      break;
            case 'E': // Edit
            case 'e':
	      if (hostSlots.host[i][0] == 0x00)
		screen_puts(3, i + 1, "                                    ");
	      screen_input(4, i + 1, hostSlots.host[i]);
	      if (hostSlots.host[i][0] == 0x00)
		screen_puts(5, i + 1, "Empty");
	      fuji_sio_write_host_slots(&hostSlots);
	      break;
            case 'D': // Drives
            case 'd':
	      i = 0;
            jump_to_devs:
	      host_done = true;
	      slot_done = false;
	      screen_puts(0, 20, "        \xD9\xA5\x19"
			  "Eject\xD9\xA8\x19Hosts\xD9\xAE\x19New          ");
	      break;
            case 0x9B: // ENTER
	      *selected_host = i;
	      if (hostSlots.host[i][0] != 0x00)
                {
		  // Write hosts
		  fuji_sio_write_host_slots(&hostSlots);
		  
		  // Mount host
		  fuji_sio_mount_host(i, &hostSlots);		  
		  ret = true;
                }
	      else
		ret = false;
	      host_done = true;
	      slot_done = true;
	      break;
            } // switch
	  
	  if (k > 0)
            {
	      bar_clear();
	      bar_show(i + 2);
	      k = 0;
            }
        }
      prev_consol = GTIA_READ.consol;
    }
  
  bar_clear();
  if (slot_done == false)
    bar_show(13);
  
  bar_clear();
  bar_show(i + 13);
  
  k = 0;
  
  // Keyboard input in DRIVES area
  while (slot_done == false)
    {
      // Quick boot
      if (GTIA_READ.consol == 0x03)
        {
	  fuji_sio_mount_all_hosts(&deviceSlots,&hostSlots);
	  fuji_sio_mount_all_devices(&deviceSlots);
	  cold_boot();
        }
      
      if (kbhit())
        {
	  k = cgetc();
	  switch (k)
            {
            case 0x1C: // ATASCII UP
            case '-':
	      if (i > 0)
		i--;
	      break;
            case 0x1D: // ATASCII DOWN
            case '=':
	      if (i < 7)
		i++;
	      break;
            case '_': // SHIFT + UP ARROW
	      color_luminanceIncrease();
	      break;
            case '|': // SHIFT + DOWN ARROW
	      color_luminanceDecrease();
	      break;
            case '\\': // SHIFT + LEFT ARROW
	      color_hueDecrease();
	      break;
            case '^': // SHIFT + RIGHT ARROW
	      color_hueIncrease();
	      break;
            case '!': // Hosts 1-7
            case '"':
            case '#':
            case '$':
            case '%':
            case '&':
            case '\'':
	      i = k - '!';
	      slot_done = true;
	      host_done = false;
	      goto rehosts_jump;
	      break;
            case '@': // Special case for host 8
	      i = 7;
	      slot_done = true;
	      host_done = false;
	      goto rehosts_jump;
	      break;
            case 'C': // Config
            case 'c':
	      host_done = true;
	      slot_done = true;
	      ret = false;
	      info_run();
	      break;
            case 'H': // Hosts
            case 'h':
	      slot_done = true;
	      host_done = false;
	      goto rehosts;
            case 'E': // Eject
            case 'e':
            doeject:
	      fuji_sio_umount_device(i);
	      screen_puts(0, i + 11, " ");
	      screen_puts(3, i + 11, "  Empty                              ");
	      memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
	      deviceSlots.slot[i].hostSlot = 0xFF;
	      fuji_sio_write_device_slots(&deviceSlots);
	      break;
            case 'N': // New disk
            case 'n':
	      screen_puts(4, i + 11, "                                    ");
	      screen_puts(0, 20, "Enter filename of new ATR image        ");
	      screen_puts(0, 21, "                                       ");
	      memset(tmp_str, 0, sizeof(tmp_str));
	      memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
	      
	      // #8bitandmore
	      retval = screen_input(4, i + 11, deviceSlots.slot[i].file);
	      if (strcmp(deviceSlots.slot[i].file, "") == 0 || retval == -1)
                {
		  fuji_sio_umount_device(i);
		  screen_puts(0, i + 11, " ");
		  screen_puts(3, i + 11, "  Empty                              ");
		  memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
		  deviceSlots.slot[i].hostSlot = 0xFF;
		  fuji_sio_write_device_slots(&deviceSlots);
		  screen_puts(0, 20, "                                       ");
		  host_done = true;
		  slot_done = false;
		  screen_puts(0, 20, "        \xD9\xA5\x19"
			      "Eject\xD9\xA8\x19Hosts\xD9\xAE\x19New          ");
		  screen_puts(0, 21, "    \xD9\x91\x8D\x98\x19"
			      "Drives \xD9\xDC\x91\x8D\x98\x19"
			      "Hosts\xD9\xA3\x19"
			      "Config");
		  break;
                }
	      //
	      
	      screen_puts(0, 20, "Which Host Slot (1-8)?                 ");
	      screen_puts(0, 21, "                                       ");
	      memset(tmp_str, 0, sizeof(tmp_str));
	      screen_input(23, 20, tmp_str);
	      deviceSlots.slot[i].hostSlot = atoi(tmp_str);
	      deviceSlots.slot[i].hostSlot -= 1;
	      fuji_sio_mount_host(deviceSlots.slot[i].hostSlot,&hostSlots);
	      screen_puts(0, 20, "Size?\xD9\x91\x19"
			  "90K  \xD9\x92\x19"
			  "130K  \xD9\x93\x19"
			  "180K  \xD9\x94\x19"
			  "360K  ");
	      screen_puts(0, 21, "     \xD9\x95\x19"
			  "720K \xD9\x96\x19"
			  "1440K \xD9\x97\x19"
			  "Custom          ");
	      memset(tmp_str, 0, sizeof(tmp_str));
	      screen_input(32, 21, tmp_str);
	      disk_type = atoi(tmp_str);
	      if (disk_type == 1)
                {
		  ns = 720;
		  ss = 128;
                }
	      else if (disk_type == 2)
                {
		  ns = 1040;
		  ss = 128;
                }
	      else if (disk_type == 3)
                {
		  ns = 720;
		  ss = 256;
                }
	      else if (disk_type == 4)
                {
		  ns = 1440;
		  ss = 256;
                }
	      else if (disk_type == 5)
                {
		  ns = 2880;
		  ss = 256;
                }
	      else if (disk_type == 6)
                {
		  ns = 5760;
		  ss = 256;
                }
	      else if (disk_type == 7)
                {
		  screen_puts(0, 20, "# Sectors?                            ");
		  screen_puts(0, 21, "                                      ");
		  memset(tmp_str, 0, sizeof(tmp_str));
		  screen_input(12, 20, tmp_str);
		  ns = atoi(tmp_str);
		  screen_puts(0, 21, "Sector size (128/256)?                ");
		  memset(tmp_str, 0, sizeof(tmp_str));
		  screen_input(24, 21, tmp_str);
		  ss = atoi(tmp_str);
                }
	      
	      memset(tmp_str, 0, sizeof(tmp_str));
	      screen_puts(0, 20, "Are you sure (Y/N)?                     ");
	      screen_puts(0, 21, "                                        ");
	      screen_input(21, 20, tmp_str);
	      
	      if ((tmp_str[0] == 'Y') || (tmp_str[0] == 'y'))
                {
		  screen_puts(0, 20, "Creating new Disk                       ");
		  screen_puts(0, 21, "                                        ");
		  fuji_sio_new_disk(i, ns, ss, &deviceSlots);
		  
		  if (OS.dcb.dstats != 1)
		    goto doeject;
		  
		  fuji_sio_write_device_slots(&deviceSlots);
		  goto rehosts;
                }
	      else
                    goto doeject;
	      break;
            } // switch
	  
	  if (k > 0)
            {
	      bar_clear();
	      bar_show(i + 13);
	      k = 0;
            }
        }
      prev_consol = GTIA_READ.consol;
    }
