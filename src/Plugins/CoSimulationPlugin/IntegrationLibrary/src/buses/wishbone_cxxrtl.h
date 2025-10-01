#ifndef Wishbone_CXXRTL_H
#define Wishbone_CXXRTL_H

// CXXRTL Wishbone interface
//
// (c) 2025, section5.se

#include "bus.h"

#include <cxxrtl/cxxrtl.h>

using namespace cxxrtl;

class WishboneCXXRTLBase
{

public:
    value<1>   *wb_clk;
    value<1>   *wb_rst;
    value<32>  *wb_addr;
    value<32>  *wb_rd_dat;
    value<32>  *wb_wr_dat;
    value<1>   *wb_we;
    value<4>   *wb_sel;
    value<1>   *wb_stb;
    value<1>   *wb_ack;
    value<1>   *wb_cyc;
    // value<1>   wb_stall = nullptr;
    uint8_t   granularity;
    uint8_t   addr_lines;
};


class WishboneCXXRTL : public WishboneCXXRTLBase, public BaseTargetBus
{
	public:
    WishboneCXXRTL() { }
    virtual void tick(bool countEnable, uint64_t steps);
    virtual void write(int width, uint64_t addr, uint64_t value);
    virtual uint64_t read(int width, uint64_t addr);
    virtual void reset();
    bool areSignalsConnected() { return true; }
    void timeoutTick(uint8_t *signal, uint8_t v, int timeout) override; // XXX REVISIT
    void timeoutTickCXXRTL(value<1> *signal, bool v, int timeout);
};
#endif
