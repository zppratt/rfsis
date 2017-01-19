#ifndef _ARPSNIFFER_HPP_
#define _ARPSNIFFER_HPP_


class arpsniffer{
public:
  void process_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
  void process_ip_packet(const u_char * , int);
  void print_ip_packet(const u_char * , int);
  void print_tcp_packet(const u_char *  , int );
  void print_udp_packet(const u_char * , int);
  void print_icmp_packet(const u_char * , int );
  void PrintData (const u_char * , int);

private:
  
};


#endif
