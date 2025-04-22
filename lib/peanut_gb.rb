# frozen_string_literal: true

require_relative "peanut_gb/version"
require_relative "peanut_gb/peanut_gb"

module PeanutGb
  class Error < StandardError; end

  class Emulator
    def rom_read(addr)
      raise NotImplementedError, "#{self.class.name}#rom_read not implemented"
    end

    def cart_ram_read(addr)
      raise NotImplementedError, "#{self.class.name}#cart_ram_read not implemented"
    end

    def cart_ram_write(addr, val)
      raise NotImplementedError, "#{self.class.name}#cart_ram_write not implemented"
    end

    def error(e, addr)
      raise NotImplementedError, "#{self.class.name}#error not implemented"
    end

    def lcd_draw_line(pixels, line)
      raise NotImplementedError, "#{self.class.name}#lcd_draw_line not implemented"
    end

    def audio_read(addr)
      raise NotImplementedError, "#{self.class.name}#audio_read not implemented"
    end

    def audio_write(addr, val)
      raise NotImplementedError, "#{self.class.name}#audio_write not implemented"
    end

    def serial_tx(tx)
      raise NotImplementedError, "#{self.class.name}#serial_tx not implemented"
    end

    def serial_rx(rx)
      raise NotImplementedError, "#{self.class.name}#serial_rx not implemented"
    end

    def bootrom_read(addr)
      raise NotImplementedError, "#{self.class.name}#bootrom_read not implemented"
    end
  end
end
