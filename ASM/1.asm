.model tiny    

.code   

org 100h

data:

jmp start

message db "Hello, world!", 10, 13, "It's alive!", '$'     

start: 

    mov dx, offset message 
    mov ah, 9
    int 21h       

    mov ax, 4c00h
    int 21h

end data