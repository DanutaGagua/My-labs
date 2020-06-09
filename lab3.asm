.model small

.stack

.data
 
raws_max equ 5
columns_max equ 6
number equ 30    

raws db ?
columns db ?      

value dw 0
num_digits db 0

str_3 db 6 dup(?), '$' 

flag_null db 0 

ten dw 10         

message_1 db "Enter matrix.", 10, 13, '$'
message_2 db "Result", 10, 13, '$' 
message_3 db "Don't enough memory for this number. You should write number from -32 768 to 32 767.", 10, 13, '$'
message_4 db "Not number. You should write number from -32 768 to 65 555.", 10, 13, '$' 
message_5 db "Don't enough memory for multiplication in one of raws.", 10, 13, '$'
message_6 db "Enter number raws from 1 to 5.", 10, 13, '$' 
message_7 db "Enter number columns from 1 to 6.", 10, 13, '$'    
                  
array dw 30 dup(?)
array_of_mul dw 5 dup (?) 
array_of_flag db 5 dup (?)
max dw 1 dup (?)  

assume ds:@data, es:@data 

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

compare macro character  
    
    cmp al, character
    je if_number
    
endm

read proc  
    
    write message_1
    
    xor ax, ax    
    mov al, raws       
    mul columns
    
    xor cx, cx
    mov cx, ax  
    
    lea di, array        ;index of array
    
    read_array: 
        
        xor si, si        ;flag if minus
        xor bx, bx        ;flag if number      
        
        mov value, 0 
        mov num_digits, 0
        
        mov flag_null, 0
        
        read_number:
        
            mov ah, 01h
            int 21h      
            
            cmp num_digits, 0
            jne compare_with_digit   
            
            cmp al, 00h
            je read_number
            
            cmp al, ' '
            je read_number
            
            cmp al, 13
            je read_number 
            
            cmp al, '+'
            je read_number
            
            cmp al, '-'
            jne compare_with_digit
            
                cmp si, 0
                jne if_minus
                
                    mov si, 1
                    
                    jmp read_number 
                    
                if_minus:
                    
                    mov si, 0   
                    
                    jmp read_number
            
            compare_with_digit:
            
                cmp al, 30h
                jl if_end_of_number
                
                cmp al, 39h
                jg if_end_of_number
                
            if_number:
            
                mov bl, al   
                sub bl, 30h
                mov ax, word ptr value  
               
                mul ten                  
                add ax, bx
               
                cmp dx, 0
                jne if_overflow 
               
                mov word ptr value, ax
                inc num_digits 
                
                jmp read_number    
               
            if_end_of_number:
            
                cmp al, 00h
                je end_loop_read_number
            
                cmp al, ' '
                je end_loop_read_number
               
                cmp al, 13
                je end_loop_read_number
                
            not_number: 
            
                write message_4
                jmp read_number   
               
            if_overflow:  
                
                write message_3
                jmp read_array
                         
    end_loop_read_number:  
    
        cmp al, 13
        jne move_to_array
        
        enter
        
        move_to_array:
        
            mov ax, value 
            
            cmp si, 0
            je move_value
            
                mov dx, -1
                imul dx  
                jc if_overflow
            
            move_value:             
                       
                mov [di], ax         
                add di, 2
                
        loop read_array 
    
    ret
        
read endp    

read_number_ proc   
    
    read_raws_message:      
        
        write message_6   
        
        read_raws:    
        
            mov ah, 01h
            int 21h     
            
            cmp al, 00h
            jne if_not_null_raw
            
            mov al, ' '
            
            if_not_null_raw:
            
                cmp al, ' '
                je read_raws 
            
                cmp al, 13
                je read_raws   
                
                cmp al, 31h
                jl read_raws_message
                
                cmp al, 35h
                jg read_raws_message  
                        
            if_number_raws:             
                
                sub al, 30h  
                mov raws, al 
        
        enter
    
    read_columns_message:
    
        write message_7 
                    
        read_columns:      
        
            mov ah, 01h
            int 21h     
            
            cmp al, 00h
            jne if_not_null_col
            
            mov al, ' '
            
            if_not_null_col:
            
                cmp al, ' '
                je read_columns 
            
                cmp al, 13
                je read_columns       
                
                cmp al, 31h
                jl read_raws_message
                
                cmp al, 36h
                jg read_raws_message   
                        
            if_number_columns:             
                
                sub al, 30h   
                mov columns, al    
                
        enter
    
    ret
        
read_number_ endp

calculate_mul proc        
    
     xor cx, cx
     
     mov cl, raws 
     
     lea di, array
     lea si, array_of_mul 
     lea bx, array_of_flag
     
     calculte_max_in_raw:  
     
        push cx    
        
        mov ax, 1   
        xor cx, cx
     
        mov cl, columns
        
        calculate_mul_in_column:  
                                              
            mov dx, word ptr [di]  
            
            imul dx       
            
            jo overflow      
            
            jnc next_raw
            
            cmp dl, 255
            jne overflow
            
            next_raw:
            
                add di, 2  
            
        loop calculate_mul_in_column   
        
        cmp dl, 255 
        jne plus
        
        mov [bx], 1
        
        plus:
        
        mov word ptr[si], ax  
        
        add si, 2 
        
        add bx, 1
        
        pop cx
     
     loop calculte_max_in_raw  
     jmp proc_end
        
     proc_end:
     
        ret  
    
calculate_mul endp 

find_max proc 
   
    xor cx, cx
     
    mov cl, raws 
   
    dec cx
    
    lea di, array_of_mul 
    
    mov ax, word ptr [di]  
    
    mov word ptr max, ax  
    
    add di, 2    
    
    cmp cx, 0
    je cx_zero 
    
    cycle:  
    
       cmp ax, word ptr [di]
       jge cycle_end
       
       mov ax, word ptr [di]  
       
       cycle_end:       
    
           add di, 2
   
    loop cycle 
    
    mov word ptr max, ax    
    
    cx_zero:
    
    ret
    
find_max endp

write_raw proc 
    
    xor cx, cx
     
    mov cl, raws 
    
    mov dx, 1
    
    lea di, array_of_mul  
    
    cycle_:  
       
        mov ax,  word ptr [di]
       
        cmp ax, max
        jne next
        
        add dl, 30h 
        
        mov ah, 02h
        int 21h
        
        sub dl, 30h   
        
        push dx
        
        mov dl, ' '
        
        mov ah, 02h
        int 21h 
        
        pop dx
         
        next:
       
            add di, 2
            inc dx 
   
    loop cycle_   
    
    
    ret
    
write_raw endp  

write_mul proc 
    
    xor cx, cx
     
    mov cl, raws 
    
    lea di, array_of_mul 
            
    lea si, array_of_flag 
    
    cycle_write_mul:    
    
        mov ax, word ptr [di]   
       
        push cx 
        push bx
        push di       
        
        lea di, str_3 
        
        xor cx, cx  
        
        mov bx, 10   
        
        cmp [si], 0
            
        je m1    
        
        mov dx, -1
        
        imul dx
        
        mov dx, ax
        
        xor ax, ax
        mov al, '-'
        
        stosb
        
        mov ax, dx 
        
    m1:   
        xor dx, dx 
        
        div bx  
           
        push DX  
           
        INC CX  
        
        TEST AX,AX
        JNZ m1      
        
    m2:   
    
        POP AX        
        
        symbol:
        
            ADD AL,'0'
          
            STOSb   
            
    loop m2  
            
        pop di     
        pop dx   
        
        mov ah, 09h
        lea dx, str_3  
        int 21h   
        
        mov dl, 10
        mov ah, 02h 
        int 21h 
    
        mov dl, 13
        mov ah, 02h 
        int 21h
        
        add di, 2        
        
        push si
        
        lea si, str_3 
        
        mov cx, 6 
        
        clear_str:
        
            mov [si], 00h
            
            add si, 1
            
        loop clear_str 
         
        pop si
        pop cx  
        
        add si, 1          
   
    loop cycle_write_mul         
    
    ret
    
write_mul endp

begin:  

    mov ax, @data
    mov ds, ax
    mov es, ax
               
    call read_number_
    call read    
    
    enter  
    
    call calculate_mul
    call find_max            
    call write_raw
    
    enter
     
    call write_mul
    
    jmp begin_end         
     
    overflow:
     
        write message_5
    
    begin_end:   

        mov ax, 4c00h
        int 21h

end begin  
