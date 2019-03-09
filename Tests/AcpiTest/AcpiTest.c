/** @file
  Test acpi support.

Copyright (c) 2018, vit9696. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/OcDevicePropertyLib.h>
#include <Library/DevicePathLib.h>
#include <Library/OcMiscLib.h>
#include <Library/OcProtocolLib.h>
#include <Library/OcAppleBootPolicyLib.h>
#include <Library/OcSmbiosLib.h>

#include <Protocol/AppleBootPolicy.h>
#include <Protocol/DevicePathPropertyDatabase.h>


#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/OcAcpiLib.h>

UINT8 PatchedSsdt8[] = {
  0x53, 0x53, 0x44, 0x54, 0x25, 0x01, 0x00, 0x00, 0x02, 0x60, 0x41, 0x50, 0x50, 0x4C, 0x45, 0x20,
  0x53, 0x73, 0x64, 0x74, 0x45, 0x43, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x49, 0x4E, 0x54, 0x4C,
  0x10, 0x12, 0x16, 0x20, 0xA0, 0x1C, 0x00, 0x15, 0x5C, 0x2F, 0x03, 0x5F, 0x53, 0x42, 0x5F, 0x50,
  0x43, 0x49, 0x30, 0x4C, 0x50, 0x43, 0x42, 0x06, 0x00, 0x15, 0x5C, 0x55, 0x4D, 0x41, 0x50, 0x01,
  0x00, 0x10, 0x43, 0x0E, 0x5C, 0x5F, 0x53, 0x42, 0x5F, 0x5B, 0x82, 0x4F, 0x08, 0x55, 0x53, 0x42,
  0x58, 0x08, 0x5F, 0x41, 0x44, 0x52, 0x00, 0x14, 0x42, 0x08, 0x5F, 0x44, 0x53, 0x4D, 0x04, 0xA0,
  0x09, 0x93, 0x6A, 0x00, 0xA4, 0x11, 0x03, 0x01, 0x03, 0xA4, 0x12, 0x4F, 0x06, 0x08, 0x0D, 0x6B,
  0x55, 0x53, 0x42, 0x53, 0x6C, 0x65, 0x65, 0x70, 0x50, 0x6F, 0x77, 0x65, 0x72, 0x53, 0x75, 0x70,
  0x70, 0x6C, 0x79, 0x00, 0x0B, 0xEC, 0x13, 0x0D, 0x6B, 0x55, 0x53, 0x42, 0x53, 0x6C, 0x65, 0x65,
  0x70, 0x50, 0x6F, 0x72, 0x74, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E, 0x74, 0x4C, 0x69, 0x6D, 0x69,
  0x74, 0x00, 0x0B, 0x34, 0x08, 0x0D, 0x6B, 0x55, 0x53, 0x42, 0x57, 0x61, 0x6B, 0x65, 0x50, 0x6F,
  0x77, 0x65, 0x72, 0x53, 0x75, 0x70, 0x70, 0x6C, 0x79, 0x00, 0x0B, 0xEC, 0x13, 0x0D, 0x6B, 0x55,
  0x53, 0x42, 0x57, 0x61, 0x6B, 0x65, 0x50, 0x6F, 0x72, 0x74, 0x43, 0x75, 0x72, 0x72, 0x65, 0x6E,
  0x74, 0x4C, 0x69, 0x6D, 0x69, 0x74, 0x00, 0x0B, 0x34, 0x08, 0x10, 0x4A, 0x04, 0x5C, 0x2F, 0x03,
  0x5F, 0x53, 0x42, 0x5F, 0x50, 0x43, 0x49, 0x30, 0x4C, 0x50, 0x43, 0x42, 0x5B, 0x82, 0x37, 0x45,
  0x43, 0x5F, 0x5F, 0x08, 0x5F, 0x48, 0x49, 0x44, 0x0C, 0x41, 0xD0, 0x0C, 0x09, 0x08, 0x5F, 0x55,
  0x49, 0x44, 0x00, 0x08, 0x5F, 0x43, 0x52, 0x53, 0x11, 0x15, 0x0A, 0x12, 0x47, 0x01, 0x62, 0x00,
  0x62, 0x00, 0x00, 0x01, 0x47, 0x01, 0x66, 0x00, 0x66, 0x00, 0x00, 0x01, 0x79, 0x00, 0x08, 0x5F,
  0x47, 0x50, 0x45, 0x0A, 0x17
};

STATIC UINT8 HpetPatchFind[] = {'H', 'P', 'E', 'T'};
STATIC UINT8 HpetPatchReplace[] = {'X', 'P', 'E', 'T'};
STATIC UINT8 HpetPatchMask[] = {0xFF, 0xFF, 0xFF, 0xFF};
STATIC
OC_ACPI_PATCH
HpetPatch = {
  .Find    = HpetPatchFind,
  .Replace = HpetPatchReplace,
  .Mask    = HpetPatchMask,
  .Size    = sizeof (HpetPatchFind),
  //
  // Replace only once.
  //
  .Count   = 1,
  .Skip    = 0,
  //
  // Only patch DSDT.
  //
  .TableSignature = EFI_ACPI_6_2_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
  .TableLength = 0,
  .OemTableId = 0
};

EFI_STATUS
EFIAPI
TestAcpi (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                  Status;
  OC_ACPI_CONTEXT                             Context;

  Status = AcpiInitContext (&Context);

  if (!EFI_ERROR (Status)) {
    AcpiLoadRegions (&Context);

    AcpiDropTable (&Context, EFI_ACPI_6_2_DMA_REMAPPING_TABLE_SIGNATURE, 0, 0);
    AcpiDropTable (&Context, EFI_ACPI_6_2_WINDOWS_SMM_SECURITY_MITIGATION_TABLE_SIGNATURE, 0, 0);
    AcpiDropTable (&Context, EFI_ACPI_6_2_WINDOWS_ACPI_EMULATED_DEVICES_TABLE_SIGNATURE, 0, 0);

    AcpiInsertTable (&Context, PatchedSsdt8, sizeof (PatchedSsdt8));

    AcpiApplyPatch (&Context, &HpetPatch);

    AcpiRelocateRegions (&Context);

    AcpiNormalizeHeaders (&Context);

    AcpiApplyContext (&Context);

    AcpiFreeContext (&Context);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINTN  Index;

  WaitForKeyPress (L"Press any key...");

  for (Index = 0; Index < SystemTable->NumberOfTableEntries; ++Index) {
    Print (L"Table %u is %g\n", (UINT32) Index, &SystemTable->ConfigurationTable[Index].VendorGuid);
  }

  Print (L"This is test app...\n");

  TestAcpi (ImageHandle, SystemTable);

  return EFI_SUCCESS;
}
