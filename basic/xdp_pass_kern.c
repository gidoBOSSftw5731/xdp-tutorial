/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/bpf.h>
#include <linux/in.h>
#include <../libbpf/src/bpf_helpers.h>

// The parsing helper functions from the packet01 lesson have moved here
#include "../common/parsing_helpers.h"
#include "../common/rewrite_helpers.h"

#ifndef memcpy
#define memcpy(dest, src, n) __builtin_memcpy((dest), (src), (n))
#endif

SEC("xdp")
int  gre_incoming(struct xdp_md *ctx)
{
	return XDP_PASS;
}

//practice
SEC("xdp")
int  drop_some_icmp6(struct xdp_md *ctx)
{
	/* this is the packet context*/
	void *data_end = (void *)(long)ctx->data_end;
	void *data = (void *)(long)ctx->data;
	struct bpf_fib_lookup fib_params = {};
	//ethernet header
	struct ethhdr *eth = data;
	// ipv6 header
	struct ipv6hdr *ip6h;
	// ipv4 header
	//struct iphdr *iph;
	__u16 h_proto;
	__u64 nh_off;
	int rc;
	/* default action is to pass */
	int action = XDP_PASS;

	/* determine if this is IP4 or IPv6 by looking at the Ethernet protocol field */
	h_proto = eth->h_proto;
	if (h_proto == bpf_htons(ETH_P_IPV6)) {
		/* IPv6 part of the code */
		struct in6_addr *src = (struct in6_addr *) fib_params.ipv6_src;
		struct in6_addr *dst = (struct in6_addr *) fib_params.ipv6_dst;

		ip6h = *data + nh_off;

		/* populate the fib_params fields to prepare for the lookup */
		//fib_params.family	= AF_INET6;
		//fib_params.flowinfo	= *(__be32 *) ip6h & IPV6_FLOWINFO_MASK;
		//fib_params.l4_protocol	= ip6h->nexthdr;
		//fib_params.sport	= 0;
		//fib_params.dport	= 0;
		//fib_params.tot_len	= bpf_ntohs(ip6h->payload_len);
		*src			= ip6h->saddr;
		*dst			= ip6h->daddr;
	} else {
		// if it isnt ipv6 then for now I dont want to handle it
		goto out;
	}

	ip6h


	// yes, goto bad, but this looks like the best way
	out:
	return action;
}

char _license[] SEC("license") = "GPL";
