# bbb-fdc
Use a BBB to connect to WD37C65C as FDC inspired by Scott Baker's pi-fdc

Why use a BeagleBoneBlack board?

Well, firstly, I had one. Secondly, it has more and more versatile IO then any Raspberry. GPIO access from userspace is fast with about 2.x MHz pin toggling. And if required, two PRU controllers can be initialized to shift high speed IO to dedicated hardware processing units. 

Not to forget the amazing BBBio_lib that allows port access to a number of initialized pins. So reading and writing bytes to a (data) port does not get any easier.  
