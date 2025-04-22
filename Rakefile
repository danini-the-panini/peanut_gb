# frozen_string_literal: true

require "bundler/gem_tasks"
require "minitest/test_task"

Minitest::TestTask.create

require "rake/extensiontask"

task build: :compile

GEMSPEC = Gem::Specification.load("peanut_gb.gemspec")

Rake::ExtensionTask.new("peanut_gb", GEMSPEC) do |ext|
  ext.lib_dir = "lib/peanut_gb"
end

task default: %i[clobber compile test]
