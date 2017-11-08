print "RadTeam Frequency Reading"

import pyvisa
import time

rm = pyvisa.ResourceManager()
inst = rm.open_resource('USB::0x0699::0x0411::C013473::INSTR')
inst.write('Measure::imm:source CH1')
inst.write('Measure::imm:Type frequency')
  
for x in range(0,60):
  #  inst.write('Measure::imm:source CH1')
  #  inst.write('Measure::imm:Type frequency')
    frequency1 = inst.query('Measure:imm:val?').rstrip('\n')
    
    print(frequency1)
    time.sleep(1)



