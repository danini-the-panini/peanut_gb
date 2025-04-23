# PeanutGb

Ruby C extension wrapping PeanutGB

## Installation

TODO: Replace `peanut_gb` with your gem name right after releasing it to RubyGems.org. Please do not do it earlier due to security reasons. Alternatively, replace this section with instructions to install your gem from git if you don't plan to release to RubyGems.org.

Install the gem and add to the application's Gemfile by executing:

```bash
bundle add peanut_gb
```

If bundler is not being used to manage dependencies, install the gem by executing:

```bash
gem install peanut_gb
```

## Usage

The gem comes with a basic exe that displays to the terminal:

```
peanut_gb path/to/rom.gb
```

The controls are as folows:

A: Z
B: X
START: Enter
SELECT: Backspace
RIGHT: Right
LEFT: Left
UP: Up
DOWN: Down
QUIT: Q
RESET: R
Toggle Interlacing: I
Toggle Frame Skip: O

You can also extend `PeanutGb::Emulator` to implement your own renderer etc.

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake test` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/danini-the-panini/peanut_gb.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
