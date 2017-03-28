#include "std_includes.hpp"

class NetworkMimic{
public:
  int clone_mac(string hwAddr);
  struct pico_device* spoof_IP();
private:
};

int NetworkMimic::clone_mac(string hwAddr) {

	HWAddress<6> hwAddress(hwAddr);

	struct ifreq ifr;
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	assert(s != -1);

	strcpy(ifr.ifr_name, "tap0");

	int count = 0;
	printf("%s\n", "Cloning mac");

	for(auto element : hwAddress) {
    printf("FOUND DATA FOR HW ADDRESS: %d\n", element);
		ifr.ifr_hwaddr.sa_data[count] = element;
		count++;
	}

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

	// Clone the Mac address
	assert(ioctl(s, SIOCSIFHWADDR, &ifr) != -1);

	printf("%s\n", "Mac address successfully cloned!");

	return EXIT_SUCCESS;
}

struct pico_device* NetworkMimic::spoof_IP(){
    struct pico_device *dev = conf.getDev(); // Our Pico Device var
    struct pico_ip4 ipaddr, netmask; // Pico uses weird conversions, so these are specific types pico uses for ip-address and netmask

    if(!dev) { // Check for error
        printf("clonemac.hpp =======> Could not spoof ip on tap device. FATAL!!!!\n");
        return NULL;
    }

    int rtn_code;

  rtn_code = pico_string_to_ipv4(conf.getIpv4_Addr().c_str(), &ipaddr.addr); // get the main server address from config parser and convert and copy to pico address
  rtn_code =  pico_string_to_ipv4(conf.getNetmask().c_str(), &netmask.addr); // Convert the netmask from our config parser to pico netmask
  rtn_code = pico_ipv4_link_del(dev, ipaddr); // Link'em all together, registering them to the IP-Stack
  rtn_code = pico_ipv4_link_add(dev, ipaddr, netmask);

  if (rtn_code == -1){
    printf("clonemac.hpp =======> Spoof failure. Device going down.");
    exit(-1);
  }

    log_debug("clonemac.hpp =======> Spoofed Device: " + conf.getTap_Device_name());
    log_debug("clonemac.hpp =======> With IP Address " + conf.getIpv4_Addr());
    log_debug("clonemac.hpp =======> And Netmask " + conf.getNetmask());

    return dev; //return our device
}
