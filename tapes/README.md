Esta pasta deve conter ficheiros .P (snapshots de memória do Sinclair ZX81) para serem carregados pelo emulador.

⚠️ Importante:
Nesta fase, o carregamento de programas NÃO é feito através do comando LOAD "..." no BASIC do ZX81.
O fluxo suportado é o carregamento pela linha de comandos (cmd), passando o nome do ficheiro .P como argumento ao arrancar o emulador.
(Não é o PowerShell.)

Exemplo (cmd):
1) Coloca o ficheiro FLIGHT.P nesta pasta (tapes\FLIGHT.P)
2) Abre a Linha de Comandos (cmd) na pasta do executável e executa:

   zx81.exe FLIGHT.P

O emulador procura o ficheiro automaticamente dentro da pasta tapes/.
O processo de carregamento é registado no ficheiro tape_log.txt (na raiz do projeto).
