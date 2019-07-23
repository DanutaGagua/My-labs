.model tiny    

.code    

.386    

point struc  

    x db 0
    y db 0 

point ends

org 100h   

begin:  
    
    jmp start

    data:
    
        old_int_9 dd 0
        old_int_8 dd 0
        
        scan_code_right_key = 4dh
        scan_code_left_key = 4bh  
        scan_code_enter = 1ch    
        
        left_move = 0
        right_move = 1   
        up_move = 2
        down_move = 3
        
        move_side db right_move    
        
        move_side_ball_horizontal db right_move 
        move_side_ball_vertical db up_move
        
        flag db 0
        flag_of_end db 0
        flag_game_over db 0         

        value_x db 21
        value_y db 5
        
        length_help_string equ 35 
        
        help_string db "<- - left, -> - right, break all bricks, exit - enter", 0
        game_end_string_lose db "You losed. Click for exit enter.", 0     
        game_end_string_win db "You winned!!! Click for exit enter.", 0 
        score_string db "Score: ", 0 
        
        score db 0 
        
        number_bricks db 200
        
        bricks point 200 dup (<40, 2>)   
              
        platform_start point<40, 22>    ; five points  
        ball point<40, 21>   
        
        delay db 3 
        current_time db 0  
        
;;;;;;;;;;;;;;;;  int_handlers      
        
int_9_handler proc  
    
    pushf  
    
    call cs:old_int_9 
    
    pusha   
    push ds
    push es 
    
    push cs  
    pop ds   
    
    get_command:     
    
        in al, 60h  
    
        cmp al, 80h
        ja exit        
        
        compare_with_left:
    
            cmp al, scan_code_left_key
            jne compare_with_right  
            
            mov dl, left_move
            mov move_side, dl   
            
            jmp exit 
        
        compare_with_right: 
        
            cmp al, scan_code_right_key
            jne compare_with_enter 
            
            mov dl, right_move
            mov move_side, dl    
            
            jmp exit
            
        compare_with_enter: 
        
            cmp al, scan_code_enter
            jne exit 
            
            mov flag_of_end, 1
            mov flag_game_over, 1    
        
    exit:  
    
        mov al,  20h
        out 20h, al 
                      
        pop es
        pop ds    
        popa  
    
        iret       
    
int_9_handler endp   

int_8_handler proc  
    
    pushf  
    
    call cs:old_int_8
    
    pusha   
    push ds
    push es 
    
    push cs  
    pop ds  
    
    cmp flag, 0
    je exit_timer   
    
    cmp flag_of_end, 1
    je exit_timer 
    
    inc current_time  
    
    mov al, current_time
    
    cmp al, delay
    jne exit_timer
    
    sub current_time, 2
    
    call refresh_screen  
    
    exit_timer:
    
        mov al,  20h
        out 20h, al 
                      
        pop es
        pop ds  
        popa  
    
    iret       
    
int_8_handler endp   

;;;;;;;;;;;;;;;;  install

install_screen proc 
    
    mov ah, 00h
    mov al, 3
    int 10h   
    
    ret

install_screen endp 

;;;;;;;;;;;;;;;;  print

print_info proc 
    
    push 0b800h  
    pop es   
    
    mov di, 3680 
    
    lea si, help_string 
    
    mov ah, 0fh   
    
    cycle: 
    
        mov al, [si]
        
        cmp al, 0
        je m1 
        
        mov es:[di], ax
        
        add di, 2
        inc si            
    
    jmp cycle 
    
    m1: 
    
    mov di, 3840     
    
    lea si, score_string 
    
    mov ah, 0fh   
    
    cycle_1: 
    
        mov al, [si]
        
        cmp al, 0
        je m2 
        
        mov es:[di], ax
        
        add di, 2
        inc si            
    
    jmp cycle_1 
    
    m2: 
    
    xor cx, cx 
    
    xor ax, ax
    
    mov al, score
    
    mov bx, 10 
    
    m3:   
        xor dx, dx 
        
        div bx  
           
        push dx  
           
        INC CX  
        
        cmp ax, 0
        JNZ m3      
        
    m4:   
    
        xor dx, dx
    
        pop dx 
        
        mov dh, 0fh      
        
        symbol:
        
            add dl, 30h
          
            mov es:[di], dx     
            
            add di, 2
            
    loop m4   
    
    exit_print_info:   
    
        ret

print_info endp

print_game_over_string  proc     
    
    push 0b800h  
    pop es   
    
    mov di, 3680   
    
    cmp number_bricks, 0
    je if_win
    
    if_lose:
        
        lea si, game_end_string_lose 
        jmp move_info
    
    if_win:
    
        lea si, game_end_string_win   
        
    move_info:    
    
    mov ah, 0fh   
    
    cycle_game_over: 
    
        mov al, [si]
        
        cmp al, 0
        je exit_print_game_over_string 
        
        mov es:[di], ax
        
        add di, 2
        inc si            
    
    jmp cycle_game_over
    
    exit_print_game_over_string:   
    
        ret
    
    ret

print_game_over_string endp

print_walls proc 
    
    push 0b800h     
    pop es   
    
    mov di, 0000  
    
    mov ah, 68h 
    mov al, ' '   
    
    mov cx, 80
    
    cycle_print_up_walls:   
        
        mov es:[di], ax
        
        add di, 2         
    
    loop cycle_print_up_walls  
    
    mov cx, 21
    
    cycle_print_walls:  
    
        mov es:[di], ax
        
        add di, 158
        
        mov es:[di], ax
        
        add di, 2         
    
    loop cycle_print_walls  
    
    ret

print_walls endp  

print_platform proc 
    
    push 0b800h  
    pop es  
    
    xor ax, ax 
    xor dx, dx
    xor bx, bx 
    xor cx, cx  
    
    mov al, platform_start.y   
    dec al   
    mov bl, 2
    mov cx, 80  
    
    mul cx   
    mul bx  
    
    mov dl, platform_start.x    
    dec dl 
    
    add ax, dx 
    add ax, dx 
    
    mov di, ax
    
    mov ah, 28h  
    mov al, ' '   
    
    mov cx, 5  
    
    cycle_print_platform:    
    
        mov es:[di], ax
        
        add di, 2   
    
    loop cycle_print_platform  
    
    ret

print_platform endp 

print_ball proc 
    
    push 0b800h  
    pop es  
    
    xor ax, ax 
    xor dx, dx
    xor bx, bx 
    xor cx, cx  
    
    mov al, ball.y   
    dec al   
    mov bl, 2
    mov cx, 80  
    
    mul cx   
    mul bx  
    
    mov dl, ball.x    
    dec dl 
    
    add ax, dx 
    add ax, dx 
    
    mov di, ax
    
    mov ah, 38h  
    mov al, ' '  
    
    mov es:[di], ax  
    
    ret

print_ball endp  

print_bricks proc 
    
    push 0b800h  
    pop es  
    
    xor ax, ax 
    xor dx, dx
    xor bx, bx 
    xor cx, cx  
    
    mov cl, number_bricks    
    
    lea si, bricks 
    
    move_brick:
    
        push cx  
    
        mov al, [si].y 
          
        cmp al, 26
        jne move_to_video_buffer 
        
        add di, 2 
        add si, 2   
        
        pop cx
        
        jmp move_brick
        
        move_to_video_buffer:
        
            dec al   
            mov bl, 2
            mov cx, 80  
    
            mul cx   
            mul bx  
    
            mov dl, [si].x    
            dec dl 
    
            add ax, dx 
            add ax, dx 
    
            mov di, ax
    
            mov ah, 48h  
            mov al, '#' 
    
            mov es:[di], ax 
        
        next_brick:  
        
            add di, 2 
            add si, 2    
    
            pop cx  
        
    loop move_brick
    
    ret
    
print_bricks endp    

;;;;;;;;;;;;;;;;  refresh

refresh_screen proc 
    
    call install_screen   
    
    call print_walls
    call print_platform
    call print_info 
    call print_ball 
    call print_bricks
    
    call refresh_ball  
    call refresh_platform 
    
    ret

refresh_screen endp   

refresh_platform proc 
    
    xor ax, ax  
    
    mov al, platform_start.x    
    
    compare_with_left_board:   
    
        cmp al, 2
        jne compare_with_right_board
        
        mov al, right_move
        mov move_side, al 
        
        jmp change_x_to_left
        
    compare_with_right_board:   
    
        cmp al, 75
        jne change_x_to_left
        
        mov al, left_move
        mov move_side, al  
        
    change_x_to_left:  
        
        mov al, left_move
        cmp move_side, al
        jne change_x_to_right
        
        dec platform_start.x  
        
        jmp exit_refresh_platform
        
    change_x_to_right:
    
        inc platform_start.x         
        
    exit_refresh_platform:    
    
        ret

refresh_platform endp   

refresh_ball proc 
    
    xor ax, ax  
    
    mov al, ball.y
    mov ah, ball.x    
        
    compare_with_ball_down_board:   
    
        cmp al, 22
        jne check
        
        mov flag_of_end, 1
        
        jmp exit_refresh_ball 
        
    check:     
        
        call change_move_ball   
        
        call change_ball_x     
        
        call change_ball_y  
        
    exit_refresh_ball:     
    
        ret

refresh_ball endp  

change_move_ball proc   
    
    xor dx, dx
    
    mov dl, ball.y
    mov dh, ball.x
    
    call change_ball_x  
    
    check_with_ball_left_board:
    
        cmp ball.x, 1
        jne check_with_ball_right_board     
        
        mov al, right_move
        mov move_side_ball_horizontal, al
    
    check_with_ball_right_board:  
    
        cmp ball.x, 80
        jne check_if_x_brick     
        
        mov al, left_move
        mov move_side_ball_horizontal, al 
        
    check_if_x_brick:  
    
        lea si, bricks  
        
        xor cx, cx
        
        mov cl, number_bricks
        
        cycle_if_brick:
           
           cmp [si].y, 26 
           jne check_brick_x
           
           add si, 2
           jmp cycle_if_brick 
           
           check_brick_x:
           
                mov al, [si].x
                cmp al, ball.x
                jne next_cycle_if_brick
           
                mov al, [si].y
                cmp al, ball.y
                jne next_cycle_if_brick
           
                mov [si].y, 26  
           
                dec number_bricks     
                
                add score, 1
           
                mov al, 1
                sub al, move_side_ball_horizontal 
                add al, left_move
           
                mov move_side_ball_horizontal, al
           
                jmp end_check_x
           
           next_cycle_if_brick:    
           
               add si, 2   
               
        loop cycle_if_brick     
    
    end_check_x: 
    
        mov ball.x, dh    
        
    call change_ball_y
    
    check_with_ball_up_board:
    
        cmp ball.y, 1
        jne check_if_platform     
        
        mov al, down_move
        mov move_side_ball_vertical, al  
        
    check_if_platform:
    
        mov al, ball.y
        
        cmp al, platform_start.y
        jne check_if_y_brick 
        
        check_board_of_platform:
        
            mov al, ball.x
        
            cmp al, platform_start.x
            jl check_if_y_brick   
            
            mov ah, platform_start.x
            add ah, 4 
            
            cmp al, ah
            jg check_if_y_brick
            
            mov move_side_ball_vertical, up_move   
            
            jmp end_check_xy
            
    check_if_y_brick:  
    
        lea si, bricks  
        
        xor cx, cx
        
        mov cl, number_bricks
        
        cycle_if_brick_y:
           
           cmp [si].y, 26 
           jne check_brick_y
           
           add si, 2
           jmp cycle_if_brick_y 
           
           check_brick_y:
           
                mov al, [si].x
                cmp al, ball.x
                jne next_cycle_if_brick_y
           
                mov al, [si].y
                cmp al, ball.y
                jne next_cycle_if_brick_y
           
                mov [si].y, 26  
           
                dec number_bricks 
                
                add score, 1
           
                mov al, 3
                sub al, move_side_ball_vertical 
                add al, up_move
           
                mov move_side_ball_vertical, al
           
                jmp end_check_xy
           
           next_cycle_if_brick_y:    
           
               add si, 2   
               
        loop cycle_if_brick_y                 
        
    end_check_y:  
    
     call change_ball_x    
        
    check_if_platform_xy:
    
        mov al, ball.y
        
        cmp al, platform_start.y
        jne check_if_xy_brick 
        
        check_board_of_platform_xy:
        
            mov al, ball.x
        
            cmp al, platform_start.x
            jl check_if_xy_brick   
            
            mov ah, platform_start.x
            add ah, 4 
            
            cmp al, ah
            jg check_if_xy_brick
            
            mov move_side_ball_vertical, up_move 
            
            mov al, 1
            sub al, move_side_ball_horizontal 
            add al, left_move   
            
            mov move_side_ball_horizontal, al
            
    check_if_xy_brick:  
    
        lea si, bricks  
        
        xor cx, cx
        
        mov cl, number_bricks
        
        cycle_if_brick_xy:
           
           cmp [si].y, 26 
           jne check_brick_xy
           
           add si, 2
           jmp cycle_if_brick_xy 
           
           check_brick_xy:
           
                mov al, [si].x
                cmp al, ball.x
                jne next_cycle_if_brick_xy
           
                mov al, [si].y
                cmp al, ball.y
                jne next_cycle_if_brick_xy
           
                mov [si].y, 26  
           
                dec number_bricks 
                
                add score, 1
           
                mov al, 3
                sub al, move_side_ball_vertical 
                add al, up_move   
           
                mov move_side_ball_vertical, al   
                
                mov al, 1
                sub al, move_side_ball_horizontal 
                add al, left_move  
                
                mov move_side_ball_horizontal, al
           
                jmp end_check_xy
           
           next_cycle_if_brick_xy:    
           
               add si, 2   
               
        loop cycle_if_brick_xy   
        
   end_check_xy:     
    
        mov ball.y, dl 
        mov ball.x, dh 
        
        cmp number_bricks, 0 
        jne exitchange_move_ball 
        
        mov flag_of_end, 1
        
        exitchange_move_ball:         
    
    ret

change_move_ball endp  

change_ball_x proc   
    
    change_ball_x_to_left:  
        
        mov al, left_move
        cmp move_side_ball_horizontal, al
        jne change_ball_x_to_right
        
        dec ball.x  
        
        jmp exit_change_ball_x
        
    change_ball_x_to_right:
    
        inc ball.x  
        
    exit_change_ball_x:      
    
        ret

change_ball_x endp  

change_ball_y  proc   
    
    change_ball_y_to_up:  
        
        mov al, up_move
        cmp move_side_ball_vertical, al
        jne change_ball_x_to_down
        
        dec ball.y 
        
        jmp exit_change_ball_y
        
    change_ball_x_to_down:
    
        inc ball.y    
        
    exit_change_ball_y:      
    
        ret

change_ball_y endp  

;;;;;;;;;;;;;;;;  fill

fill_bricks proc 
    
    lea si, bricks
    
    mov cl, 5    
    
    cycle_generate_bricks:  
    
        push cx   
        
        mov cx, 40
        
        check_cycle_bricks:   
            
            mov al, value_x
            mov [si].x, al
        
            mov al, value_y
            mov [si].y, al 
            
            add value_x, 1   
            
            add si, 2 
           
        loop check_cycle_bricks 
        
        pop cx  
        
        mov value_x, 21    
        
        add value_y, 1
        
    loop cycle_generate_bricks   
    
    ret

fill_bricks endp

;;;;;;;;;;;;;;;;  do_with_interrupts

install_interrupts proc 
    
    ;keyboard
    
    mov ah, 35h
    mov al, 09h
    int 21h     
    
    mov word ptr old_int_9, bx
    mov word ptr old_int_9+2, es    
    
    mov ah, 25h
    mov al, 09h   
    lea dx, int_9_handler 
    int 21h     
        
    ;timer
        
    mov ah, 35h
    mov al, 08h
    int 21h  
        
    mov word ptr old_int_8, bx
    mov word ptr old_int_8+2, es    
    
    mov ah, 25h
    mov al, 08h   
    lea dx, int_8_handler 
    int 21h  
    
    ret

install_interrupts endp  

return_interrupts proc 
    
    ;keyboard
        
    mov ah, 25h
    mov al, 09h    
    mov dx, word ptr cs:old_int_9+2
    mov ds, dx 
    mov dx, word ptr cs:old_int_9 
    int 21h     
        
    ;timer
        
    mov ah, 25h
    mov al, 08h    
    mov dx, word ptr cs:old_int_8+2
    mov ds, dx 
    mov dx, word ptr cs:old_int_8 
    int 21h 
    
    ret

return_interrupts endp   

;;;;;;;;;;;;;;;;  program

program proc  
    
    call install_screen 
    
    call print_walls
    call print_platform 
    call print_info 
    call print_ball 
    
    call fill_bricks
    
    call print_bricks 
    
    mov flag, 1
    
    program_cycle:
    
        cmp flag_of_end, 0
    
    je program_cycle  
    
    call install_screen
    
    call print_game_over_string  
    
    program_cycle_over:
    
        cmp flag_game_over, 0
    
    je program_cycle_over
    
    call install_screen    
    
    ret

program endp
        
    start:   
        
        call install_interrupts   
        
        call program   
        
        call return_interrupts
        
        mov ax, 4c00h
        int 21h     

end begin