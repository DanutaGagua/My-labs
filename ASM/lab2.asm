.model small

.stack 100h 

print macro out_str
    mov dl, 10
    mov ah, 02h  
    int 21h 
    mov dl, 13
    mov ah, 02h  
    int 21h 

    mov dx, offset out_str
    mov ah, 09h  
    int 21h 
endm  

compare macro character
    cmp buffer[di], character
    je if_number
endm  

.data 

number dw 0 

str db "number "

message_1 db "Don't enough memory.$"
message_2 db "Enter string.", 10, 13, '$'
message_3 db "Result.$"
 
buffer db  200 dup(?)

size equ 200  

.code

begin:  

mov ax, @data
mov ds, ax 

xor di, di
xor cx, cx


mov dx, offset message_2 
mov ah, 09h
int 21h 

read:

    mov ah, 01h
    int 21h 

    mov buffer[di], al;    

    inc di 
    inc cx

    cmp cx, size
    je error

    cmp al, 13  
jne read  

dec di

mov buffer[di], '$' 

xor di, di

mov [di], cx  

process: 

xor ax, ax  
xor bx, bx
xor dx, dx

jmp start

numlock:

    mov buffer[di], ' '

start: 
     
    cmp buffer[di], ' '
    je add_str
      
    cmp buffer[di], 00h   
    je numlock 
    
    cmp buffer[di], '$'
    je add_str             
    
    compare '0'
    compare '1'
    compare '2'
    compare '3'
    compare '4'
    compare '5'
    compare '6'
    compare '7'
    compare '8'
    compare '9'

    inc ax
    jmp if_number
    
    if_not_number:  
    
        mov bx, di
        inc bx
        jmp if_number  
        
    if_space:
    
        inc di
        
    if_first:
    
        inc bx

    if_number:
 
        inc di             
        
loop start 

cmp cx, 0
je vyvod

add_str:

    cmp ax, 0
    mov ax, 0 
    jne if_not_number
    
    cmp di, 0
    je if_first
    
    dec di
    
    cmp buffer[di], ' '
    je if_space
    
    inc di 
    
    push cx
    push di
    
    mov cx, number 
    
    add cx, 7
    
    cmp cx, size
    jg error
    
    dec cx
    
    sdvig: 
    
        mov di, cx
        sub di, 7
        
        mov dl, buffer[di]
        add di, 7  
        mov buffer[di], dl  
        
        sub di, 6 
        
        cmp bx, di 
    
    loopne sdvig   
    
    mov cx, 7
    
    mov di, bx 
    
    xor bx, bx
    
    add_number:
        
        mov dl, str[bx] 
        mov buffer[di], dl 
        
        inc bx
        inc di       
        
    loop add_number
    
    xor ax, ax 
    xor di, di
    
    add [di], 7
    
    pop di
    pop cx  
    
    add di, 7
    
    mov bx, di
    inc bx 
    
    cmp buffer[di], '$'
jne if_number
je vyvod

error: 

    print message_1 
 
    dec di
    mov buffer[di], '$'

vyvod: 

    print message_3
    print buffer  

    mov ah, 4ch  
    int 21h 

end begin                 

