import streamlit as st
from PIL import Image, ImageEnhance
from streamlit_image_comparison import image_comparison
import io

st.set_page_config(layout="wide")
st.title("Image Editor")
#default_image = "/home/savar/pix/walls/sakura.png"
default_image = "theoffice.jpg"

uploaded_file = st.file_uploader("", type=["jpg", "jpeg", "png"])

col1, col2, col3 = st.columns(3)

with col1:
    contrast = st.slider("Contrast", 0.5, 2.0, 1.0, 0.02, key="contrast")

with col2:
    brightness = st.slider("Brightness", 0.5, 2.0, 1.0, 0.02, key="brightness")

with col3:
    grayscale = st.slider("Grayscale", 0.0, 1.0, 0.5, 0.02, key="grayscale")

# load image
if uploaded_file is not None:
    image = Image.open(uploaded_file)
else:
    image = Image.open(default_image)

edited_image = image
edited_image = ImageEnhance.Color(edited_image).enhance(1 - grayscale)
edited_image = ImageEnhance.Brightness(edited_image).enhance(brightness)
edited_image = ImageEnhance.Contrast(edited_image).enhance(contrast)

col1, col2, col3 = st.columns([1, 8, 1])

with col2:  # This centers the component
    # Use CSS to force full width within the center column
    image_comparison(
        img1=image,
        img2=edited_image,
        label1="Original",
        width=10000,
        label2="Edited",
        starting_position=48,
        show_labels=True,
        make_responsive=True,
        in_memory=True,
    )

