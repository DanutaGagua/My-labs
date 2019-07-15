.model tiny 

.stack 100h

.data 

message db "Error.", 10, 13, '$'

current_hour db ?
current_minute db ?
current_second db ?  

ten dw 10

.code

enter macro  
    
    mov dl, 10
    mov ah, 02h 
    int 21h
     
    mov dl, 13
    mov ah, 02h 
    int 21h 
    
endm
    
write macro string    
     
    mov dx, offset string 
    mov ah, 9
    int 21h 
    
endm 

get_bcd proc 
    
    xor ax, ax

    pop ax
    
    div ten  
       
    push dx  
    push ax

    ret
    
get_bcd endp    

get_time proc
    
    mov ah, 02h    
    int 1ah      
    
    jc error 
    
    mov byte ptr current_hour, ch
    mov byte ptr current_minute, cl
    mov byte ptr current_second, dh  
    
    mov dl, dh
    xor dh, dh
    
    push dx    
    call get_bcd  
    
    mov dl, cl
    call get_bcd  
    
    mov dl, ch
    call get_bcd 
    
    mov cx, 6
    
    cycle: 
        mov ah, 2
        
        pop dx
        
        mov al, dl
        add al, '0'
        
        int 21h
    loop cycle  
    jmp exit
    
    error:
    
        write message
        
    exit:
    
        ret    
    
get_time endp   

begin:
    
    mov ax, @data
    mov ds, ax

    call get_time 
    
    ;call set_time
    
    ;call get_time
    
    ;call set_alarm 
    
    mov ax, 4c00h
    int 21h

end begin