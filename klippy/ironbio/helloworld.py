from extras.gcode_macro import GCodeMacro

class HelloWorld:
    def __init__(self, config):
        self.printer = config.get_printer()
        self.gcode = self.printer.lookup_object('gcode')
        self.i = -1
        self.gcode.register_command(
            'HELLOWORLD',
            self.cmd_HELLOWORLD,
            desc="Send hello to MCU"
        )
        self.mcu = self.printer.lookup_object('mcu')
        self.mcu.register_response(
            self._handle_response, 
            "helloworld_response"
        )

    def _handle_response(self, params):
        self.gcode.respond_raw(">> MCU говорит: Hello World!")

    def cmd_HELLOWORLD(self, gcmd):
         # Получаем объект MCU
        mcu = self.printer.lookup_object('mcu')
        # Создаем команду для отправки
        cmd = mcu.lookup_command("helloworld")
        # Отправляем команду
        cmd.send()
        self.gcode.respond_raw("testtesttest")
        gcmd.respond_info("Hello command sent to MCU" + str(self.i))
        self.i += 1

def load_config(config):
    return HelloWorld(config)