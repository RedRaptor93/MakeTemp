// makeTemp entry point

#define CLARA_CONFIG_CONSOLE_WIDTH 90

#include "makeTemp.h"
#include "fmt/ostream.h"
#include "clara.hpp"

#define XMKTEMP_VER             "1.0.0"
#define XMKTEMP_AUTHOR          "Pedro Rodrigues"
#define XMKTEMP_DEF_TEMPLATE    "{}.tmp"

using namespace std;
using namespace clara;
namespace fs = std::filesystem;


int main(int argc, char *argv[]) 
{
    bool dry_run = false, showHelp = false, createDir = false;
    string templ = XMKTEMP_DEF_TEMPLATE;
    fs::path baseDir = fs::temp_directory_path();
    int name_size = 11;

    auto cli = Help(showHelp)
    | Opt(dry_run)
        ["-u"]["--dry-run"]
        ("Don't create anything, just print the name")
    | Opt(createDir)
        ["-d"]["--directory"]
        ("Create a directory instead of a file")
    | Opt(baseDir, "base dir")
        ["-b"]["--base-dir"]
        ("Base directory where the file/dir will be created, defaults to your system's "
        "TMP folder")
    | Arg(templ, "name template")
    | Opt(name_size, "n# chars")
        ["-l"]["--rnd-lenght"]
        ("Number of random chars to write when a replacement field is found (min 3, max 255), 11 by default")
    ;

    auto result = cli.parse(Args(argc, argv));
    if(!result) 
    {
        fmt::print(stderr, "Error in command line: {}\n", result.errorMessage());
        return 1;
    }

    if (showHelp)
    {
        // printing 'cli' prints Clara's generated help
        fmt::print("Creates a temporary file or directory\n{}\n", cli); 
        fmt::print("Name template:\n{}\n\n", 
            TextFlow::Column(
            "Template for the new file/dir name, optionally it may contain a replacement field ('{}') "
            "that will be replaced by 'n# chars' of random characters, any other characters are taken verbatim.\n"
            "Only ONE replacement field is allowed in the template. "
            "If nothing is specified, '" XMKTEMP_DEF_TEMPLATE "' is used.\n"
            "If no replacement field is present, the template will be the file/dir name. However if a file/dir "
            "of the same name exists in 'base dir', makeTemp will fail w/o touching the file system."
        ).indent(2));
        fmt::print("Credits:\n{}\n\n",
            TextFlow::Column("makeTemp " XMKTEMP_VER " by " XMKTEMP_AUTHOR).indent(2)
        );
        return 0;
    }

    error_code ec;
    auto path = make_temp_name(templ, baseDir, name_size, ec);
    if (ec)
    {
        fmt::print(stderr, "Error: {}\n", ec.message());
        return 1;
    }

    if (!dry_run) 
    {
        ec = create_temp(path, createDir);
        if (ec)
        {
            fmt::print(stderr, "Error: {}\n", ec.message());
            return 1;
        }
    }

    // all went well, print path
    fmt::print(path.string());

    return 0;
}
