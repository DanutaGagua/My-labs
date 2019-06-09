.model tiny

.386

.code

org 80h

cmd_length db ?
cmd_line db ?

org 100h

data:

jmp start

message_1 db "Error of reading.", 10, 13, '$'
message_2 db "Files are equal.", 10, 13, '$'
message_3 db "The first file is bigger than the second.", 10, 13, '$'
message_4 db "The first file is smaller than the second.", 10, 13, '$'     

error_message_1 db "Didn't find any filename.", 10, 13, '$' 
error_message_2 db "Didn't find the second filename.", 10, 13, '$'
error_message_3 db "Didn't find the first file.", 10, 13, '$' 
error_message_4 db "Didn't find the second file.", 10, 13, '$'
error_message_5 db "The first file is bigger than 64 kB.", 10, 13, '$'
error_message_6 db "The second file is bigger than 64 kB.", 10, 13, '$'   

filename_1 db 256 dup(?)
filename_2 db 256 dup(?)  

file_length_1 dw ?    
file_length_2 dw ?   

buffer db 200, ?, 200 dup(?) 

write_enter macro  
    
    mov dl, 10
    mov ah, 02h 
    int 21h
     
    mov dl, 13
    mov ah, 02h 
    int 21h 
    
endm
    
write macro string  
    
    write_enter  
     
    mov dx, offset string 
    mov ah, 9
    int 21h 
    
endm  

get_file macro filename  
    
        call miss_space
        
        lea di, filename                        
    
        call get_filename 
    
        mov [di], 0    
    
endm

open_file macro filename 
    
    mov ah, 3dh
    mov al, 00h
    lea dx, filename 
    int 21h   
    
    mov bx, ax 
    
endm

miss_space proc  
    
    mov ax, ' '
    repz scasb   
        
    dec di
    mov si, di     
    
    ret
    
miss_space endp  

get_filename proc 
    
    move_filename:
        
        lodsb
        
        cmp al, 0dh
        je exit_proc       
        
        cmp al, ' '  
        je exit_proc
        
        stosb
        
        add dx, 1   
        
        jmp move_filename   
        
    exit_proc:    
    
        ret
   
get_filename endp

 write_length proc 
    
    xor cx, cx 
    
    m1:   
        xor dx, dx 
        
        div bx  
           
        push dx  
           
        INC CX  
        
        cmp ax, 0
        JNZ m1      
        
    m2:   
    
        xor dx, dx
    
        pop dx       
        
        symbol:
        
            add dl, 30h
          
            mov ah, 02h
            int 21h   
            
    loop m2   
    
    write_enter  
    
    ret
   
 write_length endp

start: 

    cld   
    
    xor cx, cx
    
    mov cl, cmd_length 
    cmp cx, 1
    jle not_found_file_headers  
    
    get_file_1: 
    
        dec cx     
    
        lea di, cmd_line  
        
        get_file filename_1   
        
    get_file_2: 
    
        mov di, si     
    
        xor dx, dx  
        
        cmp ax, 0dh
        je not_found_second_file_headers  
                       
        get_file filename_2                
    
        cmp dx, 1
        jle not_found_second_file_headers  
        
    open_file filename_1 
    
    jc error_of_open_file_1 
    
    xor di, di
    
    read_file_1: 
    
        mov cx, 200
        
        lea dx, buffer
        
        mov ah, 3fh
        int 21h  
        
        jc error  
        
        add di, ax
        jc overflow_1  
        
        cmp ax, cx
         
    je read_file_1
    
    mov word ptr file_length_1, di 
    
    xor ax, ax      
        
    mov ah, 3eh
    int 21h    
    
    open_file filename_2
    
    jc error_of_open_file_2   
    
    xor di, di 
    
    read_file_2:
    
        mov cx, 200
        
        lea dx, buffer
        
        mov ah, 3fh
        int 21h    
        
        jc error
        
        add di, ax
        jc overflow_2 
        
        cmp ax, cx
         
    je read_file_2
    
    mov word ptr file_length_2, di 
    
    xor ax, ax  
    
    mov ah, 3eh
    int 21h  
    
    mov bx, 10  
    
    mov ax, word ptr file_length_1
    
     call write_length  
    
    mov ax, word ptr file_length_2  
    
     call write_length    

mov ax, word ptr file_length_2    
    
    cmp word ptr file_length_1, ax
    
    jb if_less
    je if_equal
    jmp if_greater
    
    not_found_file_headers:
    
        write error_message_1
        jmp exit 
        
    not_found_second_file_headers:  
        
        write error_message_2
        jmp exit  
        
    error_of_open_file_1:
    
        write error_message_3 
        jmp exit
        
    error_of_open_file_2:
    
        write error_message_4     
        jmp exit
        
    overflow_1: 
    
        write error_message_5     
        jmp exit  
        
    overflow_2: 
    
        write error_message_6     
        jmp exit    
        
    error:
    
        write message_1      
        jmp exit        
        
    if_less: 
    
        write message_4     
        jmp exit
    
    if_equal: 
    
        write message_2    
        jmp exit
    
    if_greater: 
    
        write message_3   

    exit:

        mov ax, 4c00h
        int 21h

end data