keilpath := C:\Keil_v5\C51\BIN

main:
	${keilpath}\C51.exe SenseUnit.c DB OE BR BROWSE NOAREGS NOINTPROMOTE DEBUG OBJECTEXTEND CODE SYMBOLS INCDIR(common\src)
	${keilpath}\C51.exe isr_dmd.c DB OE BR BROWSE NOAREGS NOINTPROMOTE DEBUG OBJECTEXTEND CODE SYMBOLS INCDIR(common\src)	
	${keilpath}\C51.exe i2c.c DB OE BR BROWSE NOAREGS NOINTPROMOTE DEBUG OBJECTEXTEND CODE SYMBOLS INCDIR(common\src)
	
	${keilpath}\a51.exe startup.a51 NOMOD51 DEBUG XREF EP INCDIR(\common\src)
	${keilpath}\a51.exe common\src\si4010_rom_keil.a51 NOMOD51 DEBUG XREF EP INCDIR(\common\src)
	${keilpath}\a51.exe common\src\si4010_rom_all.a51 NOMOD51 DEBUG XREF EP INCDIR(\common\src)
	
	${keilpath}\BL51.EXE startup.obj, isr_dmd.obj, SenseUnit.obj, i2c.obj, common\src\si4010_rom_keil.obj, common\src\si4010_link.obj, common\lib\si4010_api_add_keil.lib, common\src\si4010_data.obj TO finalprog.omf PL(68) PW(78) IXREF RS (256) CODE (0x0-0X0EFF) XDATA (0X0F00-0X107F) STACK (?STACK (0x90))
	
	${keilpath}\OH51.EXE finalprog.omf
	
clean:
	del *.M51
	del *.OMF
	del *.LST
	del *.OBJ

upload:
	-winscp /script=winscpupload.txt
	
uploadrun: main upload clean
