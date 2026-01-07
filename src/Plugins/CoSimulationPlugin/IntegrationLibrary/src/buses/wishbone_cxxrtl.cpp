#include "wishbone_cxxrtl.h"
#include <cstdio>

#include "../renode_bus.h"

// CXXRTL specific synchronous wishbone variant
//
// This allows only a strobe length of one clock cycle during an
// access cycle (bus.cyc), i.e. it will not remain asserted during the
// wait period for a bus.ack.
// The reason for this is that we don't want double-writes to occur
// in the simple memory mapped range interface.

void WishboneCXXRTL::tick(bool countEnable, uint64_t steps = 1)
{
	uint16_t _irq;
	uint32_t i;

	irqack->set<uint16_t>(0);

    for(i = 0; i < steps; i++) {
        wb_clk->set<bool>(true);
        evaluateModel();
        wb_clk->set<bool>(false);
        evaluateModel();
    }

	// The problematic thing with this code is, that we might
	// miss short pulses. If that is the case, use the
	// `irqack` pin to reset a corresponding 'irq pending' pulse
	// that is set by the actual short event pulse.
	// In most cases, software resets the IRQ condition inside their
	// IRQ handler.
	_irq = irq->get<uint16_t>();

	irq_access[0] = _irq & 1;
	irq_access[1] = (_irq >> 1) & 1;
	irq_access[2] = (_irq >> 2) & 1;
	irq_access[3] = (_irq >> 3) & 1;

	irqack->set<uint16_t>(_irq);

	agent->handleInterrupts();

    if(countEnable) {
        tickCounter += steps;
    }
}

void WishboneCXXRTL::timeoutTickCXXRTL(value<1> *signal, bool v,
	int timeout = DEFAULT_TIMEOUT)
{
    while(signal->get<bool>() != v && timeout > 0) {
        tick(true);
        timeout--;
    }

    if(timeout == 0) {
        throw "Timeout waiting for ACK";
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

	// Make sure we strobe only for one cycle
	tick(true);
    wb_we->set<bool>(false);
    wb_stb->set<bool>(false);

    timeoutTickCXXRTL(wb_ack, true);

    wb_cyc->set<bool>(false);
    wb_sel->set<uint8_t>(0);

    timeoutTickCXXRTL(wb_ack, false);

}

uint64_t WishboneCXXRTL::read(int width, uint64_t addr)
{
	static char msg[] = "Unexpected read width %d\0";
	char out[80];

    if(width > granularity) {
        sprintf(out, msg, width);
        throw out;
    }

    wb_we->set<bool>(false);
    wb_sel->set<uint8_t>((1 << width) - 1);
    wb_cyc->set<bool>(true);
    wb_stb->set<bool>(true);
    wb_addr->set<uint64_t>(addr >> (32 - addr_lines));

	// Make sure we strobe only for one clock pulse

    tick(true);
    wb_stb->set<bool>(false);

    timeoutTickCXXRTL(wb_ack, true);

    wb_cyc->set<bool>(false);
    wb_sel->set<uint8_t>(0);

    uint64_t result = wb_rd_dat->get<uint64_t>();

    timeoutTickCXXRTL(wb_ack, false);

    return result;
}

void WishboneCXXRTL::reset()
{
    wb_rst->set<bool>(true);
    tick(true);
    wb_rst->set<bool>(false);
    tick(true);
}
