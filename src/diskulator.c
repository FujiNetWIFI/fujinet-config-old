/**
 * Fujinet Configurator
 *
 * Diskulator
 */

#include <stdbool.h>
#include "diskulator.h"
#include "diskulator_host.h"
#include "diskulator_select.h"
#include "diskulator_drive.h"

/**
 * Run the Diskulator
 */
void diskulator_run(void)
{
    while (diskulator_done == false)
    {
        if (diskulator_host() == true)
        {
            if (diskulator_select() == true)
            {
                diskulator_drive();
            }
        }
    }
    diskulator_boot();
}
