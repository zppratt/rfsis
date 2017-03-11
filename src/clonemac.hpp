#include <sys/ioctl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>

class clonemac{
public:
  clonemac();
  int clone_mac(string hwAddr);

private:
  //String new_mac;
};

clonemac::clonemac() {
  //read in config file to get new_mac
}

int clonemac::clone_mac(string hwAddr) {

	HWAddress<6> hwAddress(hwAddr);

	struct ifreq ifr;
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	assert(s != -1);

	/*For testing purposes new mac addr currently hardcoded*/
	strcpy(ifr.ifr_name, "tap0");

	int count = 0;
	printf("%s\n", "Cloning mac");

	for(auto element : hwAddress) {
		//printf("%02x\n", element);
		ifr.ifr_hwaddr.sa_data[count] = element;
		count++;
	}

	/*ifr.ifr_hwaddr.sa_data[0] = 0xDE;
	ifr.ifr_hwaddr.sa_data[1] = 0xAD;
	ifr.ifr_hwaddr.sa_data[2] = 0xBE;
	ifr.ifr_hwaddr.sa_data[3] = 0xEF;
	ifr.ifr_hwaddr.sa_data[4] = 0xCA;
	ifr.ifr_hwaddr.sa_data[5] = 0xFE;
	*/

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	assert(ioctl(s, SIOCSIFHWADDR, &ifr) != -1);

	printf("%s\n", "Mac address successfully cloned!");

	return EXIT_SUCCESS;
}
