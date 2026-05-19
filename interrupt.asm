 [bits 32]
extern irq0_handler
global timer_irq

timer_irq:
    pushad          
    call irq0_handler
    popad          
    iretd           