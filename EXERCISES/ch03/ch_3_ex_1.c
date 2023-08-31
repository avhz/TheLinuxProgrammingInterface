#include <stdio.h>
#include <linux/reboot.h>
#include <unistd.h>

int main()
{
    // LINUX_REBOOT_MAGIC1 = 0xfee1dead
    // Reads as: "Feel dead"
    printf("LINUX_REBOOT_MAGIC1 = 0x%08X (%d)\n", LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC1);

    // LINUX_REBOOT_MAGIC2 = 672274793
    // Linus Torvalds' birthday: 28 December 1969
    printf("LINUX_REBOOT_MAGIC2 = 0x%08X (%d)\n", LINUX_REBOOT_MAGIC2, LINUX_REBOOT_MAGIC2);

    // Try to reboot the system with incorrect magic numbers.
    sync();
    // printf("REBOOT: (%d)\n", reboot(69));
    // int syscall(SYS_reboot, 69, 420, 123, "ARGUMENT");

    return 0;
}