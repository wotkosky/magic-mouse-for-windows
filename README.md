# Magic Mouse 2 for Windows - Precision Touchpad Driver

Fork do [mac-precision-touchpad](https://github.com/imbushuo/mac-precision-touchpad) com suporte ao **Apple Magic Mouse 2** via Bluetooth como Windows Precision Touchpad.

Transforma a superfície touch do Magic Mouse 2 em um Precision Touchpad do Windows, habilitando scroll com 2 dedos, gestos com 3 dedos (Task View, trocar desktop virtual), etc.

## Guia Completo de Instalação (do zero)

### Pré-requisitos

| Componente | Versão | Observação |
|---|---|---|
| Windows | 10/11 x64 | Precisa desativar Secure Boot |
| Visual Studio | 2022 Community ou Build Tools | Workload C++ Desktop |
| Windows SDK | 10.0.22621.0+ | Vem com o VS |
| Windows Driver Kit (WDK) | 10.0.26100+ | [Download](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) |

### Setup automatizado

```cmd
:: Instala VS Build Tools + WDK automaticamente (~8GB)
setup_build_env.bat
```

Ou instale manualmente:

1. **Visual Studio 2022** com workload "Desktop development with C++"
2. **Windows SDK** (incluído no VS installer)
3. **WDK** - baixe e instale separadamente. Após instalar, instale também a extensão VSIX do WDK que fica em:
   ```
   C:\Program Files (x86)\Windows Kits\10\Vsix\VS2022\10.0.26100.0\WDK.vsix
   ```

### Toolset do WDK (importante!)

O build precisa de um Platform Toolset customizado. Ele já está incluído nos arquivos `Toolset.props` e `Toolset.targets`, mas precisa ser copiado para:

```
C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Microsoft\VC\v170\Platforms\x64\PlatformToolsets\WindowsKernelModeDriver10.0\
```

Copie os dois arquivos (`Toolset.props` e `Toolset.targets`) para essa pasta.

### Fix do WDK para MSBuild 17.14+

Se o build falhar com erro de comparação numérica no `WindowsDriver.OS.props`, substitua o arquivo:

```
C:\Program Files (x86)\Windows Kits\10\build\WindowsDriver.OS.props
```

pelo `WindowsDriver.OS.props.fixed` incluído neste repo. O fix corrige comparações numéricas que o MSBuild 17.14+ não aceita mais.

---

## Build

```cmd
do_build3.bat
```

Ou manualmente:

```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" ^
  src\AmtPtpHidFilter\AmtPtpHidFilter.vcxproj ^
  /p:Configuration=Release /p:Platform=x64 ^
  /p:TreatWarningAsError=false /v:minimal
```

**Output:** `src\AmtPtpHidFilter\build\AmtPtpHidFilter\x64\Release\AmtPtpHidFilter.sys` (~18KB)

---

## Instalação do Driver

### Passo 1: Desativar Secure Boot

1. Reinicie o PC e entre na BIOS/UEFI (geralmente F2, F12, ou Del durante o boot)
2. Encontre a opção **Secure Boot** e desative
3. Salve e reinicie no Windows

### Passo 2: Habilitar Test Signing

```cmd
:: Execute como Administrador
enable_testsigning.bat
```

Ou manualmente:

```cmd
bcdedit /set testsigning on
```

**REINICIE o PC.** Após reiniciar, você verá uma marca d'água "Test Mode" no canto inferior direito da tela. Isso é normal.

### Passo 3: Instalar o Driver

```cmd
:: Execute como Administrador
install_driver_final.bat
```

O script faz automaticamente:
1. Verifica se test signing está ativo
2. Cria certificado auto-assinado `CN=LiteMagicMouse Test`
3. Exporta e instala o certificado em Root e TrustedPublisher
4. Gera catálogo (.cat) com `Inf2Cat`
5. Assina .sys e .cat com SHA256
6. Instala o driver via `pnputil`

### Passo 4: Parear o Magic Mouse 2

1. Abra **Configurações > Bluetooth**
2. Coloque o Magic Mouse 2 em modo de pareamento (desligue e ligue)
3. Pareie normalmente
4. O driver filtro deve carregar automaticamente sobre o dispositivo HID

---

## Detalhes Técnicos do Magic Mouse 2 Bluetooth

### Identificação

| Propriedade | Valor |
|---|---|
| Vendor ID (BT) | `0x004C` (Apple) |
| Product ID | `0x0269` |
| Hardware ID | `HID\{00001124-0000-1000-8000-00805f9b34fb}_VID&0001004c_PID&0269` |

### HID Collections

O Magic Mouse 2 expõe 3 HID collections via Bluetooth:

| Collection | Função | O que o driver faz |
|---|---|---|
| Col01 | Mouse principal | **Filtro instalado aqui** - intercepta e converte para PTP |
| Col02 | Mouse secundário | Mantém como fallback (mouse básico sempre funciona) |
| Col03 | Vendor-specific (0xFF00) | **Nulificado** pelo driver |

### Superfície Touch

| Propriedade | Valor |
|---|---|
| Dimensões físicas | 90.56mm x 51.52mm |
| Range X | -1100 a 1258 |
| Range Y | -1589 a 2047 |
| Report ID (multitouch) | `0x12` |
| Header size | 14 bytes |
| Finger record size | 8 bytes cada |
| Max fingers | 5 (PTP limit) |

### Feature Report para Multitouch

Para habilitar os reports de multitouch via Bluetooth, o driver envia o feature report:

```
Report ID: 0xF1
Data: 0x02, 0x01
```

### Formato do Finger Record (8 bytes)

Cada dedo reportado pelo Magic Mouse 2 usa 8 bytes com coordenadas X/Y de 12 bits:

```c
typedef struct _MAGIC_MOUSE_FINGER {
    UINT8  AbsX_Low;           // 8 bits baixos de X
    UINT8  AbsX_High_AbsY_Low; // 4 bits altos de X + 4 bits baixos de Y
    UINT8  AbsY_High;          // 8 bits altos de Y
    UINT8  TouchMajor;         // eixo maior da elipse de toque
    UINT8  TouchMinor;         // eixo menor da elipse de toque
    UINT8  Size;               // tamanho do contato
    UINT8  Pressure;           // pressão
    UINT8  Orientation_Id;     // orientação (bits 0-3) + contact ID (bits 4-7)
} MAGIC_MOUSE_FINGER;
```

Decodificação:
```c
X = AbsX_Low | ((AbsX_High_AbsY_Low & 0x0F) << 8)  // sign-extend 12 bits
Y = (AbsX_High_AbsY_Low >> 4) | (AbsY_High << 4)    // sign-extend 12 bits
ContactID = (Orientation_Id >> 4) & 0x0F
IsTouching = (TouchMajor > 0) ou (Size > 0)
```

---

## Arquivos Modificados (vs. upstream)

| Arquivo | Descrição |
|---|---|
| `include/Metadata/MagicMouse2.h` | **NOVO** - Descritor HID PTP para Magic Mouse 2 |
| `include/Device.h` | Constantes MM2 (PID, header, finger size, coords, touch states) |
| `include/HidDevice.h` | Struct `MAGIC_MOUSE_FINGER` (8 bytes) |
| `include/Metadata/StaticHidRegistry.h` | Registro do descritor MM2 |
| `Device.c` | Configuração MM2 BT (dimensões, feature report) |
| `Input.c` | Parsing finger records MM2 (12-bit X/Y, contact ID, touch state) |
| `Hid.c` | Serving descritores PTP para MM2 |
| `AmtPtpHidFilter.inf` | Hardware IDs do MM2 BT |
| `include/Trace.h` | WPP tracing desabilitado (stubs) |
| `AmtPtpHidFilter.vcxproj` | WDK paths, WPP disabled |

---

## Troubleshooting

### BSOD / Tela Azul

Se o driver causar BSOD:
1. Boot em **Safe Mode** (segure Shift ao clicar Reiniciar)
2. Abra CMD como Admin
3. Remova o driver:
   ```cmd
   pnputil /enum-drivers | findstr "AmtPtpHidFilter"
   :: Anote o nome publicado (ex: oem42.inf)
   pnputil /delete-driver oem42.inf /uninstall /force
   ```
4. Reinicie normalmente

### Driver não carrega após parear

1. Abra o **Gerenciador de Dispositivos**
2. Encontre o Magic Mouse em "Dispositivos de Interface Humana"
3. Verifique se o Hardware ID corresponde ao INF:
   ```
   HID\{00001124-0000-1000-8000-00805f9b34fb}_VID&0001004c_PID&0269
   ```
4. Clique com botão direito > Atualizar driver > Procurar no computador > Escolher em uma lista
5. Selecione "Apple PTP HID Filter"

### Test signing não ativa

- **Erro "protegido pela política de Inicialização Segura"**: Secure Boot está ativo. Desative na BIOS.
- Após desativar, rode `bcdedit /set testsigning on` e reinicie.

### Verificar se o driver está carregado

```powershell
# Lista drivers carregados
driverquery /v | findstr "AmtPtp"

# Verificar no Device Manager via PowerShell
Get-PnpDevice | Where-Object { $_.FriendlyName -like "*Apple*" -or $_.FriendlyName -like "*Magic*" }
```

### Desinstalar o driver completamente

```cmd
:: Listar drivers instalados
pnputil /enum-drivers | findstr -A 3 "AmtPtp"

:: Remover (substitua oem XX pelo número correto)
pnputil /delete-driver oemXX.inf /uninstall /force

:: Desativar test signing (opcional)
bcdedit /set testsigning off
```

---

## Licença

- Driver filtro (KMDF): [MIT](LICENSE-MIT.md)
- Baseado no [mac-precision-touchpad](https://github.com/imbushuo/mac-precision-touchpad) por imbushuo

## Créditos

- [imbushuo/mac-precision-touchpad](https://github.com/imbushuo/mac-precision-touchpad) - Projeto original
- Lite Tecnologia - Adaptação para Magic Mouse 2
