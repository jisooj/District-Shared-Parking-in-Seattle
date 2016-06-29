#!/usr/bin/env python2
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Mon May  9 18:11:22 2016
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from grc_gnuradio import blks2 as grc_blks2
from optparse import OptionParser
import math
import osmosdr
import time


class top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")

        ##################################################
        # Variables
        ##################################################
        self.variable_slider_0 = variable_slider_0 = .846
        self.test = test = .005
        self.shift = shift = .906
        self.samp_rate_0 = samp_rate_0 = 1.2e6
        self.samp_rate = samp_rate = 1.2e6/4
        self.pows = pows = 1.3
        self.lpf = lpf = .724
        self.go = go = 0.564
        self.gm = gm = 1.61
        self.centre_freq = centre_freq = 439.95e6

        ##################################################
        # Blocks
        ##################################################
        self.rtlsdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.rtlsdr_source_0.set_sample_rate(samp_rate_0)
        self.rtlsdr_source_0.set_center_freq(439.9e6, 0)
        self.rtlsdr_source_0.set_freq_corr(0, 0)
        self.rtlsdr_source_0.set_dc_offset_mode(0, 0)
        self.rtlsdr_source_0.set_iq_balance_mode(2, 0)
        self.rtlsdr_source_0.set_gain_mode(False, 0)
        self.rtlsdr_source_0.set_gain(20, 0)
        self.rtlsdr_source_0.set_if_gain(10, 0)
        self.rtlsdr_source_0.set_bb_gain(10, 0)
        self.rtlsdr_source_0.set_antenna("", 0)
        self.rtlsdr_source_0.set_bandwidth(0, 0)
          
        self.low_pass_filter_1 = filter.fir_filter_fff(10, firdes.low_pass(
        	1, samp_rate, 2.56e3*lpf, (2.56e3/2)*lpf, firdes.WIN_HAMMING, 6.76))
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(4, (firdes.low_pass_2(1,samp_rate_0,100e3,50e3,40)), 0, samp_rate_0)
        self.digital_clock_recovery_mm_xx_1 = digital.clock_recovery_mm_ff(11.6439*(1+test), 0.25*0.175*0.175*go, 0.5, 0.175*gm, 0.005*variable_slider_0)
        self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
        self.blocks_add_const_vxx_0 = blocks.add_const_vff((-12*shift, ))
        self.blks2_tcp_sink_0 = grc_blks2.tcp_sink(
        	itemsize=gr.sizeof_char*1,
        	addr="127.0.0.1",
        	port=9000,
        	server=False,
        )
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(10)
        self.analog_pwr_squelch_xx_0 = analog.pwr_squelch_cc(-40*pows, .001, 0, False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_pwr_squelch_xx_0, 0), (self.analog_quadrature_demod_cf_0, 0))    
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.low_pass_filter_1, 0))    
        self.connect((self.blocks_add_const_vxx_0, 0), (self.digital_clock_recovery_mm_xx_1, 0))    
        self.connect((self.digital_binary_slicer_fb_0, 0), (self.blks2_tcp_sink_0, 0))    
        self.connect((self.digital_clock_recovery_mm_xx_1, 0), (self.digital_binary_slicer_fb_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.analog_pwr_squelch_xx_0, 0))    
        self.connect((self.low_pass_filter_1, 0), (self.blocks_add_const_vxx_0, 0))    
        self.connect((self.rtlsdr_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    


    def get_variable_slider_0(self):
        return self.variable_slider_0

    def set_variable_slider_0(self, variable_slider_0):
        self.variable_slider_0 = variable_slider_0

    def get_test(self):
        return self.test

    def set_test(self, test):
        self.test = test
        self.digital_clock_recovery_mm_xx_1.set_omega(11.6439*(1+self.test))

    def get_shift(self):
        return self.shift

    def set_shift(self, shift):
        self.shift = shift
        self.blocks_add_const_vxx_0.set_k((-12*self.shift, ))

    def get_samp_rate_0(self):
        return self.samp_rate_0

    def set_samp_rate_0(self, samp_rate_0):
        self.samp_rate_0 = samp_rate_0
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass_2(1,self.samp_rate_0,100e3,50e3,40)))
        self.rtlsdr_source_0.set_sample_rate(self.samp_rate_0)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.low_pass_filter_1.set_taps(firdes.low_pass(1, self.samp_rate, 2.56e3*self.lpf, (2.56e3/2)*self.lpf, firdes.WIN_HAMMING, 6.76))

    def get_pows(self):
        return self.pows

    def set_pows(self, pows):
        self.pows = pows
        self.analog_pwr_squelch_xx_0.set_threshold(-40*self.pows)

    def get_lpf(self):
        return self.lpf

    def set_lpf(self, lpf):
        self.lpf = lpf
        self.low_pass_filter_1.set_taps(firdes.low_pass(1, self.samp_rate, 2.56e3*self.lpf, (2.56e3/2)*self.lpf, firdes.WIN_HAMMING, 6.76))

    def get_go(self):
        return self.go

    def set_go(self, go):
        self.go = go
        self.digital_clock_recovery_mm_xx_1.set_gain_omega(0.25*0.175*0.175*self.go)

    def get_gm(self):
        return self.gm

    def set_gm(self, gm):
        self.gm = gm
        self.digital_clock_recovery_mm_xx_1.set_gain_mu(0.175*self.gm)

    def get_centre_freq(self):
        return self.centre_freq

    def set_centre_freq(self, centre_freq):
        self.centre_freq = centre_freq


if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = top_block()
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()
