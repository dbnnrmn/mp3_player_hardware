import os
import cairosvg


os.system('kicad-cli sch export pdf mp3_test_board.kicad_sch --black-and-white -o out/schematic.pdf')


os.system('kicad-cli pcb export gerbers mp3_test_board.kicad_pcb -l F.Cu,B.Cu,In1.Cu,In2.Cu,F.Adhesive,B.Adhesive,F.Paste,B.Paste,F.Silkscreen,B.Silkscreen,F.Mask,B.Mask,Edge.Cuts -o out')
os.system('kicad-cli pcb export pdf mp3_test_board.kicad_pcb -l F.Fab,Edge.Cuts -o "out/top.pdf"')
os.system('kicad-cli pcb export drill mp3_test_board.kicad_pcb -o out/')


os.system('kicad-cli pcb export svg  mp3_test_board.kicad_pcb -l B.Fab,Edge.Cuts --black-and-white --exclude-drawing-sheet -m -o out/bottom.svg')
cairosvg.svg2pdf(url='out/bottom.svg', write_to='out/bottom.pdf')


os.system('kicad-cli pcb export step mp3_test_board.kicad_pcb -o out/pcb.step --subst-models')
