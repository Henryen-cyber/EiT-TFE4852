import matplotlib
import matplotlib.pyplot as plt
import numpy as np

matplotlib.font_manager.get_font_names()

font = {'family' : 'Times New Roman',
        'weight' : 'normal',
        'size'   : 12}

plt.rc('font', **font)

species = (
    "Mathias",
    "Emily",
    "Sebastian",
    "Arlid",
    "Henry",
    "Simon"
)
weight_counts = {
    "Plass": np.array([7, 7, 7, 7, 7, 7]),
    "Potensiale": np.array([3, 3, 3, 3, 3, 3, 3]),
}

width = 1;

fig, ax = plt.subplots()
bottom = np.zeros(6)

for boolean, weight_count in weight_counts.items():
    p = ax.bar(species, weight_count, width, label=boolean, bottom=bottom, ec='black')
    bottom += weight_count

ax.set_title("Hvor mye plass tar hvert medlem i gruppen?")
ax.set_ylabel('Plassbruk')
ax.legend(loc="upper right")

ax.axes.get_yaxis().set_visible(False)

plt.tight_layout()
plt.show()

