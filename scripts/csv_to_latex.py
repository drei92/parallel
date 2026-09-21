import re

import pandas as pd
from pathlib import Path

RESULTADOS = Path("resultados")
TABLAS = Path("informe/tablas")
MAIN_TEX = Path("informe/main.tex")


def restaurar_marcador(placeholder, caption_regex):
    if not MAIN_TEX.exists():
        return
    texto = MAIN_TEX.read_text(encoding="utf-8")
    patron = re.compile(
        r"\\begin\{table\}\[htbp\]\n\\centering\n\\caption\{" + caption_regex + r"\}.*?\\end\{table\}\n",
        re.DOTALL,
    )
    texto = patron.sub(placeholder + "\n\n", texto)
    MAIN_TEX.write_text(texto, encoding="utf-8")


def insertar_en_main(placeholder, contenido):
    if not MAIN_TEX.exists():
        return
    texto = MAIN_TEX.read_text(encoding="utf-8")
    if placeholder in texto:
        texto = texto.replace(placeholder, contenido)
        MAIN_TEX.write_text(texto, encoding="utf-8")


def crear_tabla(csv_path, salida, caption, label):
    df = pd.read_csv(csv_path)
    agrupado = df.groupby(["algoritmo", "N", "p"])["tiempo"].mean().reset_index()

    secuencial = (
        agrupado[agrupado["algoritmo"] == "secuencial"][["N", "tiempo"]]
        .rename(columns={"tiempo": "t1"})
    )
    mpi = (
        agrupado[agrupado["algoritmo"] == "mpi"][["N", "p", "tiempo"]]
        .rename(columns={"tiempo": "tp"})
    )
    merged = mpi.merge(secuencial, on="N")
    merged["speedup"] = merged["t1"] / merged["tp"]
    merged["eficiencia"] = merged["speedup"] / merged["p"]

    Ns = sorted(merged["N"].unique())
    ps = sorted(merged["p"].unique())
    n_cols = len(ps) + 1

    with open(salida, "w", encoding="utf-8") as f:
        f.write("\\begin{table}[htbp]\n")
        f.write("\\centering\n")
        f.write(f"\\caption{{{caption}}}\n")
        f.write(f"\\label{{{label}}}\n")
        f.write("\\begin{tabular}{c" + "c" * len(ps) + "}\n")
        f.write("\\toprule\n")
        f.write("N / p & " + " & ".join(str(p) for p in ps) + " \\\\\n")

        # Tiempo
        f.write("\\midrule\n")
        f.write(f"\\multicolumn{{{n_cols}}}{{c}}{{Tiempo (s)}} \\\\\n")
        f.write("\\midrule\n")
        for N in Ns:
            fila = [str(N)]
            for p in ps:
                val = merged[(merged["N"] == N) & (merged["p"] == p)]["tp"].values
                fila.append(f"{val[0]:.6e}" if len(val) else "-")
            f.write(" & ".join(fila) + " \\\\\n")

        # Speedup
        f.write("\\midrule\n")
        f.write(f"\\multicolumn{{{n_cols}}}{{c}}{{Speedup}} \\\\\n")
        f.write("\\midrule\n")
        for N in Ns:
            fila = [str(N)]
            for p in ps:
                val = merged[(merged["N"] == N) & (merged["p"] == p)]["speedup"].values
                fila.append(f"{val[0]:.3f}" if len(val) else "-")
            f.write(" & ".join(fila) + " \\\\\n")

        # Eficiencia
        f.write("\\midrule\n")
        f.write(f"\\multicolumn{{{n_cols}}}{{c}}{{Eficiencia}} \\\\\n")
        f.write("\\midrule\n")
        for N in Ns:
            fila = [str(N)]
            for p in ps:
                val = merged[(merged["N"] == N) & (merged["p"] == p)]["eficiencia"].values
                fila.append(f"{val[0]:.3f}" if len(val) else "-")
            f.write(" & ".join(fila) + " \\\\\n")

        f.write("\\bottomrule\n")
        f.write("\\end{tabular}\n")
        f.write("\\end{table}\n")


if __name__ == "__main__":
    TABLAS.mkdir(parents=True, exist_ok=True)
    crear_tabla(
        RESULTADOS / "matvec_resultados.csv",
        TABLAS / "matvec.tex",
        "Resultados experimentales de multiplicación matriz-vector",
        "tab:matvec",
    )
    crear_tabla(
        RESULTADOS / "oddeven_resultados.csv",
        TABLAS / "oddeven.tex",
        "Resultados experimentales de Odd-Even Sort",
        "tab:oddeven",
    )

    # Restaurar marcadores en main.tex por si ya habian sido reemplazados
    restaurar_marcador("%%TABLA_MATVEC%%", r"Resultados experimentales de multiplicaci.n matriz-vector")
    restaurar_marcador("%%TABLA_ODDEVEN%%", r"Resultados experimentales de Odd-Even Sort")

    # Insertar las tablas directamente en main.tex
    insertar_en_main("%%TABLA_MATVEC%%", (TABLAS / "matvec.tex").read_text(encoding="utf-8"))
    insertar_en_main("%%TABLA_ODDEVEN%%", (TABLAS / "oddeven.tex").read_text(encoding="utf-8"))

    print("Tablas generadas en informe/tablas/ y actualizadas en informe/main.tex")
