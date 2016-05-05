snips = {}
labels = []
def read_file(fname, snips, labels)
  buf = File.read(fname)
  lines = buf.split("\n")
  section = ""
  snips[section] = {}
  lines.each {|line|
    if /\/\/\s*START\s+(?<name>\S+)/ =~ line ||
        /\/\/\s*ALL\s+(?<name>\S+)/ =~ line
      if labels.size > 0
        snips[labels[-1][0]][labels[-1][1]] += "#{name}\\n"
      end
      labels.push([section, name])
    end
    if labels.size > 0 && !(/\/\/\s*SKIP/ =~ line)
      snips[labels[-1][0]][labels[-1][1]] = "" if !snips[labels[-1][0]][labels[-1][1]]
      line_text = line.gsub(/\s*\/\/.*?$/, "")
      if (line_text =~ /^\s*$/) && (line !~ /^\s*$/)
        line_text = ""
      else
        line_text = "#{line_text}\\n"
      end
      snips[labels[-1][0]][labels[-1][1]] += line_text
    end
    if /\/\/\s*SECTION\s+(?<new_section>\S+)/ =~ line
      section = new_section
      snips[section] = {}
    end
    if /\/\/\s*END\s+(?<name>\S+)/ =~ line ||
        /\/\/\s*ALL\s+(?<name>\S+)/ =~ line
      if labels[-1] != [section, name]
        puts "Error, found END '[#{section}, #{name}]' but expected '#{labels[-1]}'"
      end
      labels.pop
    end
  }
  if labels.size > 0
    puts "Error, labels #{labels} left over"
  end
end

"templates/BnTemplate.h templates/TemplateProxy.cpp templates/BpTemplate.h templates/TemplateStubs.cpp templates/ITemplate.h templates/Template.vts".split.each {|fname|
  read_file(fname, snips, labels)
}

File.open("snippets.cpp", "w") {|file|
  file.write "// AUTO GENERATED - ruby snippets.rb\n"
  file.write "#include \"snippets.h\"\n"
  file.write "SnipMapMap snippets_cpp {\n"
  snips.each {|section, snip_dict|
    file.write "  {\"#{section}\",{\n"
    snip_dict.each {|name, snip|
      file.write "    {std::string(\"#{name}\"),\"#{snip.gsub('"','\"')}\"},\n"
    }
    file.write "  }},"
  }
  file.write("};\n")
}
File.open("snippets.h", "w") {|file|
  file.write "// AUTO GENERATED - ruby snippets.rb\n"
  file.write "#include <map>\n#include <string>\n"
  file.write "auto cmp = [](const std::string& a, const std::string&b) {\n"
  file.write "   return a.compare(b) < 0; };\n"
  file.write "typedef std::map<std::string, std::string> SnipMap;\n" 
  file.write "typedef std::map<std::string, SnipMap> SnipMapMap;\n" 
  file.write "extern SnipMapMap snippets_cpp;\n"
}
puts "Snippets are converted."
