bits 32

global load_page_directory
global enable_paging

; Loads the physical address of the page directory into the CR3 register.
load_page_directory:
    mov eax, [esp+4]
    mov cr3, eax
    ret

; Enables paging by setting the paging bit in the CR0 register.
enable_paging:
    mov eax, cr0
    or eax, 0x80000000  ; Set the PG (paging) bit
    mov cr0, eax
    ret
