import csv
import matplotlib.pyplot as plt

channels = ["Fp1_uV", "Fp2_uV", "Cz_uV", "O1_uV", "O2_uV"]
time = []
values = {name: [] for name in channels}

with open("build/eeg.csv", newline="") as file:
    for row in csv.DictReader(file):
        time.append(float(row["time_seconds"]))
        for name in channels:
            values[name].append(float(row[name]))

plt.style.use("dark_background")
colors = ["#ff6b8a", "#ffae66", "#a59bff", "#5ee0c0", "#59bfff"]

fig, axes = plt.subplots(
    len(channels), 1, figsize=(12, 7),
    sharex=True, layout="constrained"
)

fig.patch.set_facecolor("#0b1017")

for ax, name, color in zip(axes, channels, colors):
    ax.set_facecolor("#101722")
    ax.plot(time, values[name], color=color, linewidth=1.0)
    ax.axvspan(1.0, 1.2, color="#ffd36b", alpha=0.10)
    ax.set_ylabel(name.replace("_uV", ""), rotation=0, labelpad=25)
    ax.grid(color="#8da1b9", alpha=0.13)
    ax.spines[:].set_color("#526070")

axes[-1].set_xlabel("Time (seconds)")
axes[-1].set_xlim(0, 3)
fig.suptitle("deltahaze (simulated EEG)", fontsize=16)
fig.savefig("build/generated_images/deltahaze-preview.png", dpi=180)
plt.show()