/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x80000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x10000
}

SECTIONS
{
}

SECTIONS
{
  . = ALIGN(4);
  .mem_section_dummy_ram :
  {
  }
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
  .log_dynamic_data :
  {
    PROVIDE(__start_log_dynamic_data = .);
    KEEP(*(SORT(.log_dynamic_data*)))
    PROVIDE(__stop_log_dynamic_data = .);
  } > RAM

} INSERT AFTER .data;

SECTIONS
{
  .mem_section_dummy_rom :
  {
  }
  .log_const_data :
  {
    PROVIDE(__start_log_const_data = .);
    KEEP(*(SORT(.log_const_data*)))
    PROVIDE(__stop_log_const_data = .);
  } > FLASH
    .nrf_balloc :
  {
    PROVIDE(__start_nrf_balloc = .);
    KEEP(*(.nrf_balloc))
    PROVIDE(__stop_nrf_balloc = .);
  } > FLASH


  .mode_table : AT(0x00074000)
  {
    KEEP(*(SORT(.mode_table*)))
  } > FLASH

  .static_light : AT(0x00075000)
  {
    KEEP(*(SORT(.static_light*)))
  } > FLASH

  .img0 : AT(0x00076000)
  {
    KEEP(*(SORT(.img0*)))
  } > FLASH

  .img1 : AT(0x00077000)
  {
    KEEP(*(SORT(.img1*)))
  } > FLASH

  .img2 : AT(0x00078000)
  {
    KEEP(*(SORT(.img2*)))
  } > FLASH

  .img3 : AT(0x00079000)
  {
    KEEP(*(SORT(.img3*)))
  } > FLASH

  .img4 : AT(0x0007A000)
  {
    KEEP(*(SORT(.img4*)))
  } > FLASH

  .img5 : AT(0x0007B000)
  {
    KEEP(*(SORT(.img5*)))
  } > FLASH

} INSERT AFTER .text

INCLUDE "nrf_common.ld"
