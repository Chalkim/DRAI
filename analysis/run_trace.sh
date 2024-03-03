mkdir ./trace
./trace_reader ../simulation/mix/mix.tr "sip=0x0b000401"| grep " n:0" | awk '{print $4}' > queue_
