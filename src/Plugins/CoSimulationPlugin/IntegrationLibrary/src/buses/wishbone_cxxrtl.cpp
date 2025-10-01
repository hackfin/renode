#include "wishbone_cxxrtl.h"
#include <cstdio>

void WishboneCXXRTL::tick(bool countEnable, uint64_t steps = 1)
{
    for(uint32_t i = 0; i < steps; i++) {
        wb_clk->set<bool>(true);
        evaluateModel();
        wb_clk->set<bool>(false);
        evaluateModel();
    }

    if(countEnable) {
        tickCounter += steps;
    }
}

void WishboneCXXRTL::timeoutTickCXXRTL(value<1> *signal, bool v, int timeout = DEFAULT_TIMEOUT)
{
    do {
        tick(true);
        timeout--;
    }
    while(signal->get<bool>() != v && timeout > 0);

    if(timeout == 0) {
        throw "Timeout waiting for signal";
    }
}

// This unvisited function is only here for compatibility to the base class
void WishboneCXXRTL::timeoutTick(uint8_t *signal, uint8_t v, int timeout)
{
	
}

void WishboneCXXRTL::write(int width, uint64_t addr, uint64_t val)
{
    if(width < granularity ) {
        char out[80];
        char msg[] = "Unexpected write width %d";
        sprintf(out, msg, width);
        throw out;
    }

    wb_we->set<bool>(true);
    wb_sel->set<uint8_t>((1 << width) - 1);
    wb_cyc->set<bool>(true);
    wb_stb->set<bool>(true);

    wb_addr->set<uint64_t>(addr >> (32 - addr_lines));
    wb_wr_dat->set<uint64_t>(val);

    timeoutTickCXXRTL(wb_ack, true);

    wb_stb->set<bool>(false);
    wb_cyc->set<bool>(false);
    wb_we->set<bool>(false);
    wb_sel->set<uint8_t>(0);

    timeoutTickCXXRTL(wb_ack, false);
}

uint64_t WishboneCXXRTL::read(int width, uint64_t addr)
{
	static char msg[] = "Unexpected read width %d\0"; // we sprintf to self, because width is never longer than 2 digits
	char out[80];

    if(width < granularity) {
        sprintf(out, msg, width);
        throw msg;
    }


    wb_we->set<bool>(false);
    wb_sel->set<uint8_t>((1 << width) - 1);
    wb_cyc->set<bool>(true);
    wb_stb->set<bool>(true);
    wb_addr->set<uint64_t>(addr >> (32 - addr_lines));

    timeoutTickCXXRTL(wb_ack, true);

    wb_cyc->set<bool>(false);
    wb_stb->set<bool>(false);
    wb_sel->set<uint8_t>(0);

    timeoutTickCXXRTL(wb_ack, false);

    uint64_t result = wb_rd_dat->get<uint64_t>();

    return result;
}

void WishboneCXXRTL::reset()
{
    wb_rst->set<bool>(true);
    tick(true);
    wb_rst->set<bool>(false);
    tick(true);
}
