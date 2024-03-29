#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int port=is_mmio(addr);
  if(port==-1)
	  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
	  return mmio_read(addr,len,port);
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int port=is_mmio(addr);
  if(port==-1)
	  memcpy(guest_to_host(addr), &data, len);
  else
	  mmio_write(addr,len,data,port);
}

paddr_t page_translate(vaddr_t addr){
	if(!(cpu.cr0.paging))
		return addr;
	uint32_t dir=((addr)>>22)&0x3ff;
	uint32_t tab=((addr)>>12)&0x3ff;
	uint32_t offset=addr&0xfff;
	uint32_t cr3off=cpu.cr3.page_directory_base<<12;
    uint32_t temp1=paddr_read(cr3off+(dir<<2),4);
	assert(temp1&0x1);
	uint32_t temp2=paddr_read((temp1&0xfffff000)+(tab<<2),4);
	assert(temp2&0x1);
	return (temp2&0xfffff000)|offset;
	/* PDE pde,*pdir;PTE pte,*ptab;
	pdir=(PDE*)(intptr_t)paddr_read(cr3off,4);
	pde.val=paddr_read((intptr_t)&pdir[dir],4);
    assert(pde.present);
	pde.accessed=1;
	ptab=(PTE *)(intptr_t)paddr_read(pde.page_frame<<12,4);
	pte.val=paddr_read((intptr_t)&ptab[tab],4);
	assert(pte.present);
	return (pte.page_frame<<12)|offset;*/
}

uint32_t vaddr_read(vaddr_t addr, int len) {
   /* if(cpu.cr0.paging)
		assert(0);*/
	if((addr>>12)!=((addr+len-1)>>12)){
		int l1=4096-(addr&0xfff);
		int l2=len-l1;
		paddr_t addr1=page_translate(addr);
		paddr_t addr2=page_translate(addr+l1);
		uint32_t ret1=paddr_read(addr1,l1);
		uint32_t ret2=paddr_read(addr2,l2);
		return (ret1+(ret2<<(l1<<3)));
	}
	else{
		paddr_t  paddr=page_translate(addr);
		return paddr_read(paddr, len);
	}
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
	if((addr>>12)!=((addr+len-1)>>12)){
		int l1=4096-(addr&0xfff);
		int l2=len-l1;
		paddr_t addr1=page_translate(addr);
		paddr_t addr2=page_translate(addr+l1);
		paddr_write(addr1,data<<(1<<(l1*8-1)),l1);
		paddr_write(addr2,data>>(l1<<3),l2);
	}
	else{	
		paddr_t paddr=page_translate(addr);
		paddr_write(paddr, data, len);
   }
}
