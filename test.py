import streamlit as st

# Initialize session state for page navigation
if "page" not in st.session_state:
    st.session_state.page = "welcome"
    
# Function to change pages
def go_to(page_name):
    st.session_state.page = page_name

# ------------------- PAGE 1: WELCOME -------------------
if st.session_state.page == "welcome":
    st.title("🎬 Welcome to Movie Ticket Booking System")
    st.write(" ")
    st.write(" ")
    st.write("### Your simple movie booking app")
    
    if st.button("Get Started"):
        go_to("signin")

# ------------------- PAGE 2: SIGN IN -------------------
elif st.session_state.page == "signin":
    st.title("🔐 Sign In")
    
    username = st.text_input("Enter your name:")
    
    if st.button("Login"):
        if username.strip() != "":
            st.session_state.username = username
            go_to("book")
        else:
            st.warning("Please enter your name before logging in.")

# ------------------- PAGE 3: BOOK TICKETS -------------------
elif st.session_state.page == "book":
    st.title("🍿 Book Your Movie Ticket")
    st.write(f"Welcome, {st.session_state.username}!")

    st.write("### Select a movie to book:")

    movies = ["Inception", "Avatar", "Interstellar", "Jurassic Park"]

    for m in movies:
        if st.button(f"Book Ticket for {m}"):
            st.session_state.selected_movie = m
            go_to("done")

# ------------------- PAGE 4: TICKET BOOKED -------------------
elif st.session_state.page == "done":
    st.title("✅ Ticket Booked Successfully!")
    st.write(f"You have booked a ticket for {st.session_state.selected_movie}.")
    st.write("Enjoy your movie! 🍿🎥")

    if st.button("Back to Movie List"):
        go_to("book")