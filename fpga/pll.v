/**
 * PLL configuration
 *
 * This Verilog module was generated automatically
 * using the icepll tool from the IceStorm project.
 * Use at your own risk.
 *
 * Given input frequency:       100.000 MHz
 * Requested output frequency:   24.000 MHz
 * Achieved output frequency:    23.958 MHz
 */
/* verilator lint_off PINMISSING */
module pll(
	input  clock_in,
	output clock_out
	);

SB_PLL40_CORE #(
		.FEEDBACK_PATH("SIMPLE"),
		.DIVR(4'b0010),		// DIVR =  2
		.DIVF(7'b0010110),	// DIVF = 22
		.DIVQ(3'b101),		// DIVQ =  5
		.FILTER_RANGE(3'b011)	// FILTER_RANGE = 3
	) uut (
		.RESETB(1'b1),
		.BYPASS(1'b0),
		.REFERENCECLK(clock_in),
		.PLLOUTCORE(clock_out)
		);
/* verilator lint_on PINMISSING */
endmodule
