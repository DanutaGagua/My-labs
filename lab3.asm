.model small

.stack

.data
 
raws_max equ 5
columns_max equ 6
number equ 30    

raws db ?
columns db ? 

str_1 db "32768"
str_2 db "32767"  

string db 5 dup(?)

str_3 db 6 dup(?), '$' 

flag_null db 0 

ten db 10

message_1 db "Enter matrix.", 10, 13, '$'
message_2 db "Result", 10, 13, '$' 
message_3 db "Don't enough memory for this number. You should write number from -32 768 to 32 767.", 10, 13, '$'
message_4 db "Not number. You should write number from -32 768 to 32 767.", 10, 13, '$' 
message_5 db "Don't enough memory for multiplication in one of raws.", 10, 13, '$'
message_6 db "Enter number raws from 1 to 5.", 10, 13, '$' 
message_7 db "Enter number columns from 1 to 6.", 10, 13, '$'     
message_8 db "Not number.", 10, 13, '$'
                  
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
    
    enter  
     
    mov dx, offset string 
    mov ah, 9
    int 21h 
    
endm  

compare macro character  
    
    cmp al, character
    je if_number
    
endm

compare_raws macro character  
    
    cmp al, character
    je if_number_raws
    
endm

compare_columns macro character  
    
    cmp al, character
    je if_number_columns
    
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
        
        xor dx, dx        ;flag if minus
        xor bx, bx        ;flag if number 
        xor si, si        ;index of string   
        
        mov flag_null, 0
        
        read_number:
        
            mov ah, 01h
            int 21h     
            
            cmp al, 00h
            jne if_not_null
            
            mov al, ' '
            
            if_not_null:
            
                cmp al, '-'
                je if_minus
                
                cmp al, '+'
                je if_plus 
            
                cmp al, ' '
                je compare_string 
            
                cmp al, 13
                je compare_string    
                
                cmp si, 5
                je if_overflow 
            
                cmp al, '0'
                je if_null
                 
                compare '1'
                compare '2'
                compare '3'
                compare '4' 
                compare '5'
                compare '6'
                compare '7'
                compare '8'
                compare '9'
            
                jmp not_number 
              
                if_minus:  
                
                    cmp bx, 0  
                    jne not_number 
                    
                    cmp flag_null, 0 
                    jne not_number
                
                    cmp dx, 1
                    je even_numbered 
                    
                    inc dx
                    jmp if_plus 
                    
                    even_numbered:
                        
                        dec dx 
                        
                    if_plus:   
                        
                        cmp bx, 0  
                        jne not_number 
                        
                        cmp flag_null, 0 
                        jne not_number
                        je read_number 
                    
                    if_null:
                        
                        mov flag_null, 1 
                        
                        cmp bx, 0
                        je read_number 
                        
                    if_number:
                    
                        inc bx
                        
                        mov string[si], al  
                        
                        inc si
                    
                    jmp read_number  
            
                not_number:
            
                    write message_4
                    jmp restart
        
                if_overflow:
                
                    write message_3 
                
                restart:
                    
                    dec si
                    
                    clear_string:   
                    
                        cmp si, 0
                        jle read_array
                    
                        mov string[si], 00h
                        
                        dec si
                        
                        cmp si, -1    
                        
                    jne clear_string
                
                jmp read_array      
                
    compare_string:  
    
        
    
        cmp bx, 0   
        
        jne if  
        
        cmp flag_null, 0 
        
        
        je restart
        
        add di, 2
        
        jmp end_p
        
        if:
        
        cmp al, 13
        jne start     
        
        push dx
        
        enter
        
        pop dx
        
        start:
    
        cmp si, 5
        jl move_to_array    
        
        push si  
        push di  
        
        cld
        
        cmp dx, 1
        je if_negative  
        
        lea si, str_2
        jmp compare  
        
        if_negative:
        
            lea si, str_1 
        
        compare: 
         
            lea di, string    
    
            push cx
            
            mov cx, 5  
                             
            repe cmpsb     
             
            pop cx
            
            dec si
            dec di 
            
            mov al, [si] 
            sub al, 30h
            
            mov ah, [di] 
            sub ah, 30h
            
            cmp al, ah   
             
            pop di
            pop si    
            
            jl if_overflow  
    
    move_to_array:     
    
        mov ax, 0   
        
        push dx   
    
            xor si, si    
            xor dx, dx
            
            move:
        
                mov dl, string[si]
                sub dl, 30h  
                
                push dx    
                
                xor dx, dx
                
                mov dl, ten
        
                mul dx
                
                pop dx
            
                add ax, dx
        
                inc si  
            
                cmp si, bx 
            
            jne move  
            
            pop dx   
            
            
           cmp dx, 1
           jne if_positive
           
           mov dx, -1   
           
           imul dx 
           
           if_positive:
            
            mov [di], ax 
        
            add di, 2  
            
            
            
    end_p:
        
    loop restart 
    
    ret
        
read endp    

read_number_ proc    
    
    mov ah, 9 
    mov dx, offset message_6
    int 21h    
    
    read_raws:  
        
        
            mov ah, 01h
            int 21h     
            
            cmp al, 00h
            jne if_not_null_1
            
            mov al, ' '
            
            if_not_null_1:
            
                cmp al, ' '
                je read_raws 
            
                cmp al, 13
                je read_raws    
                 
                compare_raws '1'
                compare_raws '2'
                compare_raws '3'
                compare_raws '4' 
                compare_raws '5'
                compare_raws '6'
                compare_raws '7'
                compare_raws '8'
                compare_raws '9'
            
                write message_8
                jmp read_raws    
                        
                if_number_raws:             
                
                    sub al, 30h
                    
                    cmp al, raws_max   
                    jg read_raws
                    
                    xor ah, ah 
                    
                    mov raws, al     
                    
                    
    
    write message_7 
                    
    read_columns:  
        
        
            mov ah, 01h
            int 21h     
            
            cmp al, 00h
            jne if_not_null_2
            
            mov al, ' '
            
            if_not_null_2:
            
                cmp al, ' '
                je read_columns 
            
                cmp al, 13
                je read_columns    
                 
                compare_columns '1'
                compare_columns '2'
                compare_columns '3'
                compare_columns '4' 
                compare_columns '5'
                compare_columns '6'
                compare_columns '7'
                compare_columns '8'
                compare_columns '9'
            
                write message_8
                jmp read_columns    
                        
                if_number_columns:             
                
                    sub al, 30h
                    
                    cmp al, columns_max   
                    jg read_columns  
                    
                    xor ah, ah 
                    
                    mov columns, al   
    
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